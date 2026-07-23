/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-scsi modern PCI (OASIS virtio-scsi).
 * Pure C, dual license (MIT OR Apache-2.0). No GPL. No Linux virtio source.
 *
 * Queues (fixed OASIS indices):
 *   q0 control — TMF soft path (kick+poll; soft-accept when no HBA)
 *   q1 event   — async events (post at probe; soft poll + repost)
 *   q2 request — command I/O (req [+ data-out] → resp [+ data-in])
 *
 * Soft path (bring-up, no live HBA):
 *   g_fSoft after failed/absent probe — TMF/stats/event_poll soft;
 *   CDB I/O stays with scsi_mid soft LUN (submit returns -1).
 *
 * Product markers (serial): "virtio-scsi: ready …" / "soft-armed";
 * I/O via scsi_mid/scsi_door.
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/virtio.h>
#include <gj/virtio_scsi.h>

/* Local aliases for queue indices (match public GJ_VIRTIO_SCSI_Q_*). */
#define VIRTIO_SCSI_Q_CONTROL GJ_VIRTIO_SCSI_Q_CONTROL
#define VIRTIO_SCSI_Q_EVENT   GJ_VIRTIO_SCSI_Q_EVENT
#define VIRTIO_SCSI_Q_REQUEST GJ_VIRTIO_SCSI_Q_REQUEST

/* Bring-up queue geometry + blocking poll budget. */
#define VIRTIO_SCSI_Q_SIZE       64u
#define VIRTIO_SCSI_POLL_SPINS   20000000u
#define VIRTIO_SCSI_CTRL_SPINS   2000000u
#define VIRTIO_SCSI_EVENT_SZ     256u
#define VIRTIO_SCSI_EVENT_SPINS  64u /* soft non-blocking poll budget */

/* Response codes (public OASIS virtio-scsi) */
#define VIRTIO_SCSI_S_OK                 0u
#define VIRTIO_SCSI_S_OVERRUN            1u
#define VIRTIO_SCSI_S_ABORTED            2u
#define VIRTIO_SCSI_S_BAD_TARGET         3u
#define VIRTIO_SCSI_S_RESET              4u
#define VIRTIO_SCSI_S_BUSY               5u
#define VIRTIO_SCSI_S_TRANSPORT_FAILURE  6u
#define VIRTIO_SCSI_S_TARGET_FAILURE     7u
#define VIRTIO_SCSI_S_NEXUS_FAILURE      8u
#define VIRTIO_SCSI_S_FAILURE            9u
#define VIRTIO_SCSI_S_FUNCTION_SUCCEEDED 10u
#define VIRTIO_SCSI_S_FUNCTION_REJECTED  11u
#define VIRTIO_SCSI_S_INCORRECT_LUN      12u

/* Task attribute: SIMPLE (OASIS = 0) */
#define VIRTIO_SCSI_S_SIMPLE 0u

/* Controlq type: TMF */
#define VIRTIO_SCSI_T_TMF 0u

/* Product kind marker from virtio_pci kind_from_device (scsi modern/transitional). */
#define VIRTIO_SCSI_KIND 6u

/* ---- OASIS request / response shapes (clean-room public layout) ---------- */

struct virtio_scsi_req_cmd {
    u8  aLun[8];
    u64 u64Id;
    u8  u8TaskAttr;
    u8  u8Prio;
    u8  u8Crn;
    u8  aCdb[32];
} __attribute__((packed));

struct virtio_scsi_resp_cmd {
    u32 u32SenseLen;
    u32 u32Residual;
    u16 u16StatusQualifier;
    u8  u8Status;
    u8  u8Response;
    u8  aSense[96];
} __attribute__((packed));

struct virtio_scsi_ctrl_tmf_req {
    u32 u32Type;
    u32 u32Subtype;
    u8  aLun[8];
    u64 u64Tag;
} __attribute__((packed));

struct virtio_scsi_ctrl_tmf_resp {
    u8 u8Response;
} __attribute__((packed));

struct virtio_scsi_event {
    u32 u32Event;
    u8  aLun[8];
    u32 u32Reason;
} __attribute__((packed));

/* ---- Driver state -------------------------------------------------------- */

static struct gj_virtio_dev *g_pScsi;
static struct gj_virtq       g_qCtrl;  /* q0 control */
static struct gj_virtq       g_qEvent; /* q1 event */
static struct gj_virtq       g_qReq;   /* q2 request0 */
static int                   g_fReady;
static int                   g_fSoft;  /* soft queue path when no HBA */
static int                   g_fEventPosted;
static u32                   g_u32IoCount;
static u32                   g_u32IoFail;
static u32                   g_u32CtrlOk;
static u32                   g_u32CtrlFail;
static u32                   g_u32EventCount;
static u32                   g_u32LastResponse;
static u32                   g_u32LastResidual;
static u32                   g_u32LastScsiStatus;

/* Single outstanding command buffers (identity-mapped BSS). */
static struct virtio_scsi_req_cmd      g_Req __attribute__((aligned(16)));
static struct virtio_scsi_resp_cmd     g_Resp __attribute__((aligned(16)));
static struct virtio_scsi_ctrl_tmf_req g_TmfReq __attribute__((aligned(16)));
static struct virtio_scsi_ctrl_tmf_resp g_TmfResp __attribute__((aligned(16)));
static u8 g_aData[GJ_VIRTIO_SCSI_DATA_MAX] __attribute__((aligned(16)));
static u8 g_aEvent[VIRTIO_SCSI_EVENT_SZ] __attribute__((aligned(16)));

/* ---- Soft helpers -------------------------------------------------------- */

static void
soft_arm(void)
{
    g_fSoft = 1;
    g_fReady = 0;
    g_pScsi = NULL;
    g_fEventPosted = 0;
    kprintf("virtio-scsi: soft-armed (ctrl+event+req soft path, no HBA)\n");
}

static void
stats_clear(void)
{
    g_u32IoCount = 0;
    g_u32IoFail = 0;
    g_u32CtrlOk = 0;
    g_u32CtrlFail = 0;
    g_u32EventCount = 0;
    g_u32LastResponse = 0;
    g_u32LastResidual = 0;
    g_u32LastScsiStatus = 0;
}

static int
soft_tmf_ok(u32 u32Subtype)
{
    /* Soft-accept abort/reset family; reject query-only subtypes. */
    switch (u32Subtype) {
    case GJ_VIRTIO_SCSI_TMF_ABORT_TASK:
    case GJ_VIRTIO_SCSI_TMF_ABORT_TASK_SET:
    case GJ_VIRTIO_SCSI_TMF_CLEAR_ACA:
    case GJ_VIRTIO_SCSI_TMF_CLEAR_TASK_SET:
    case GJ_VIRTIO_SCSI_TMF_I_T_NEXUS_RESET:
    case GJ_VIRTIO_SCSI_TMF_LOGICAL_UNIT_RESET:
        return 1;
    default:
        return 0;
    }
}

/**
 * Post one device-write event buffer on q1 (soft post path).
 * Idempotent when already posted. Returns 0 on success / already posted.
 */
static int
event_post(void)
{
    if (!g_fReady) {
        return -1;
    }
    if (g_fEventPosted) {
        return 0;
    }
    memset(g_aEvent, 0, sizeof(g_aEvent));
    if (virtio_q_add(&g_qEvent, (gj_paddr_t)(gj_vaddr_t)g_aEvent,
                     sizeof(g_aEvent), 1) < 0) {
        return -1;
    }
    g_fEventPosted = 1;
    virtio_q_kick(&g_qEvent);
    return 0;
}

/* ---- Probe --------------------------------------------------------------- */

/*
 * Probe path: find first scsi → modern PCI caps → features → ctrl/event/req qs
 * → post event buffer → DRIVER_OK.
 * On any hard failure arms soft path (g_fSoft=1, g_fReady=0) so ctrl/event/req
 * soft APIs remain usable without a partial ready state.
 */
int
virtio_scsi_probe(void)
{
    u32 i;
    u32 c;
    gj_status_t st;

    g_pScsi = NULL;
    g_fReady = 0;
    g_fSoft = 0;
    g_fEventPosted = 0;
    stats_clear();

    c = virtio_dev_count();
    /* modern 0x1048, transitional 0x1004, or product kind==6 */
    for (i = 0; i < c; i++) {
        struct gj_virtio_dev *pDev = virtio_dev_get(i);

        if (pDev == NULL) {
            continue;
        }
        if (pDev->u16Device == GJ_VIRTIO_PCI_DEV_SCSI_MODERN ||
            pDev->u16Device == 0x1004u || pDev->u32Kind == VIRTIO_SCSI_KIND) {
            g_pScsi = pDev;
            break;
        }
    }
    if (g_pScsi == NULL) {
        kprintf("virtio-scsi: no device\n");
        soft_arm();
        return -1;
    }

    st = virtio_pci_setup(g_pScsi);
    if (st != GJ_OK || g_pScsi->pCommon == NULL) {
        kprintf("virtio-scsi: pci setup failed %d\n", (int)st);
        soft_arm();
        return -1;
    }
    /* Prefer V1; fall back to transitional (empty want mask) */
    st = virtio_negotiate(g_pScsi, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        st = virtio_negotiate(g_pScsi, 0);
        if (st != GJ_OK) {
            kprintf("virtio-scsi: negotiate failed %d\n", (int)st);
            soft_arm();
            return -1;
        }
    }

    /* q0 control — TMF soft path */
    st = virtio_q_setup(g_pScsi, &g_qCtrl, (u16)VIRTIO_SCSI_Q_CONTROL,
                        (u16)VIRTIO_SCSI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-scsi: control q failed %d\n", (int)st);
        soft_arm();
        return -1;
    }
    /* q1 event — async notifications (soft poll/repost) */
    st = virtio_q_setup(g_pScsi, &g_qEvent, (u16)VIRTIO_SCSI_Q_EVENT,
                        (u16)VIRTIO_SCSI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-scsi: event q failed %d\n", (int)st);
        soft_arm();
        return -1;
    }
    /* q2 request0 — command submission */
    st = virtio_q_setup(g_pScsi, &g_qReq, (u16)VIRTIO_SCSI_Q_REQUEST,
                        (u16)VIRTIO_SCSI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-scsi: request q failed %d\n", (int)st);
        soft_arm();
        return -1;
    }

    /* Post one event buffer (device-write); I/O still works if this fails */
    if (event_post() != 0) {
        kprintf("virtio-scsi: initial event post failed\n");
    }

    virtio_set_status(g_pScsi,
                      (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                           GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    g_fReady = 1;
    g_fSoft = 0;
    /* Product marker: greppable ready line with queue inventory */
    kprintf("virtio-scsi: ready bus=%x slot=%x (ctrl+event+req) event_posted=%d\n",
            (unsigned)g_pScsi->u8Bus, (unsigned)g_pScsi->u8Slot,
            g_fEventPosted);
    return 0;
}

int
virtio_scsi_ready(void)
{
    return g_fReady;
}

int
virtio_scsi_soft_active(void)
{
    return g_fSoft && !g_fReady;
}

/* ---- Request q2 soft path ------------------------------------------------ */

int
virtio_scsi_submit(struct gj_scsi_request *pReq)
{
    i32 i32Len;
    u32 cbData;

    /* Soft-only: CDB path is scsi_mid soft LUN; do not claim transport. */
    if (!g_fReady) {
        if (g_fSoft) {
            g_u32IoFail++;
        }
        return -1;
    }
    if (pReq == NULL || pReq->cdb.u8CdbLen == 0 || pReq->cdb.u8CdbLen > 32) {
        g_u32IoFail++;
        return -1;
    }

    /* Soft reap any stale used entries before single-outstanding push. */
    (void)virtio_q_reap(&g_qReq, 4u);

    memset(&g_Req, 0, sizeof(g_Req));
    memset(&g_Resp, 0xff, sizeof(g_Resp)); /* poison; device overwrites */
    /*
     * Single-level LUN addressing (virtio-scsi): byte0=1, byte1=LUN.
     */
    g_Req.aLun[0] = 1;
    g_Req.aLun[1] = (u8)(pReq->u32Lun & 0xff);
    g_Req.u64Id = (u64)g_u32IoCount + (u64)g_u32IoFail + 1ull;
    g_Req.u8TaskAttr = VIRTIO_SCSI_S_SIMPLE;
    memcpy(g_Req.aCdb, pReq->cdb.aCdb, pReq->cdb.u8CdbLen);

    cbData = pReq->cbData;
    if (cbData > sizeof(g_aData)) {
        cbData = sizeof(g_aData);
    }
    memset(g_aData, 0, sizeof(g_aData));
    if (cbData > 0 && pReq->pData != NULL && !pReq->fDataIn) {
        memcpy(g_aData, pReq->pData, cbData);
    }

    /*
     * Descriptor order (OASIS virtio-scsi request queue):
     *   device-readable:  req [+ data-out]
     *   device-writable:  resp [+ data-in]
     * No-data: req (R) + resp (W).
     * Data-in:  req (R) + resp (W) + data (W).
     * Data-out: req (R) + data (R) + resp (W).
     */
    if (cbData == 0) {
        if (virtio_q_add2(&g_qReq,
                          (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                          (gj_paddr_t)(gj_vaddr_t)&g_Resp, (u32)sizeof(g_Resp),
                          1) < 0) {
            g_u32IoFail++;
            return -1;
        }
    } else if (pReq->fDataIn) {
        if (virtio_q_add3(&g_qReq,
                          (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                          (gj_paddr_t)(gj_vaddr_t)&g_Resp, (u32)sizeof(g_Resp),
                          1,
                          (gj_paddr_t)(gj_vaddr_t)g_aData, cbData, 1) < 0) {
            g_u32IoFail++;
            return -1;
        }
    } else {
        if (virtio_q_add3(&g_qReq,
                          (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                          (gj_paddr_t)(gj_vaddr_t)g_aData, cbData, 0,
                          (gj_paddr_t)(gj_vaddr_t)&g_Resp, (u32)sizeof(g_Resp),
                          1) < 0) {
            g_u32IoFail++;
            return -1;
        }
    }
    virtio_q_kick(&g_qReq);
    i32Len = virtio_q_poll(&g_qReq, VIRTIO_SCSI_POLL_SPINS);
    if (i32Len < 0) {
        kprintf("virtio-scsi: timeout\n");
        g_u32IoFail++;
        return -1;
    }

    g_u32LastResponse = (u32)g_Resp.u8Response;
    g_u32LastResidual = g_Resp.u32Residual;
    g_u32LastScsiStatus = (u32)g_Resp.u8Status;
    pReq->u8Status = g_Resp.u8Status;
    {
        u32 cb = g_Resp.u32SenseLen;

        if (cb > sizeof(pReq->sense.aSense)) {
            cb = sizeof(pReq->sense.aSense);
        }
        if (cb > 0 && cb < 256) {
            memcpy(pReq->sense.aSense, g_Resp.aSense, cb);
            pReq->sense.u8SenseLen = (u8)cb;
        }
    }
    if (cbData > 0 && pReq->pData != NULL && pReq->fDataIn) {
        /* Residual soft clamp: device may leave trailing bytes unused. */
        u32 cbCopy = cbData;

        if (g_Resp.u32Residual > 0 && g_Resp.u32Residual < cbData) {
            cbCopy = cbData - g_Resp.u32Residual;
        }
        memcpy(pReq->pData, g_aData, cbCopy);
    }
    if (g_Resp.u8Response != VIRTIO_SCSI_S_OK) {
        kprintf("virtio-scsi: response=%u status=%u sense_len=%u residual=%u\n",
                (unsigned)g_Resp.u8Response, (unsigned)g_Resp.u8Status,
                g_Resp.u32SenseLen, g_Resp.u32Residual);
        g_u32IoFail++;
        return -1;
    }
    if (g_Resp.u8Status != GJ_SCSI_GOOD) {
        kprintf("virtio-scsi: scsi status=%u residual=%u\n",
                (unsigned)g_Resp.u8Status, g_Resp.u32Residual);
        g_u32IoFail++;
        return -1;
    }
    g_u32IoCount++;
    return 0;
}

u32
virtio_scsi_io_count(void)
{
    return g_u32IoCount;
}

u32
virtio_scsi_fail_count(void)
{
    return g_u32IoFail;
}

/* ---- Control q0 soft path (TMF) ------------------------------------------ */

int
virtio_scsi_ctrl_tmf(u32 u32Subtype, u32 u32Lun, u64 u64Tag)
{
    i32 i32Len;

    /* Soft-only path: accept abort/reset family as nop success. */
    if (!g_fReady) {
        if (g_fSoft && soft_tmf_ok(u32Subtype)) {
            g_u32CtrlOk++;
            g_u32LastResponse = VIRTIO_SCSI_S_FUNCTION_SUCCEEDED;
            return 0;
        }
        g_u32CtrlFail++;
        return -1;
    }

    if (u32Subtype > GJ_VIRTIO_SCSI_TMF_QUERY_TASK_SET) {
        g_u32CtrlFail++;
        return -1;
    }

    (void)virtio_q_reap(&g_qCtrl, 2u);

    memset(&g_TmfReq, 0, sizeof(g_TmfReq));
    memset(&g_TmfResp, 0xff, sizeof(g_TmfResp));
    g_TmfReq.u32Type = VIRTIO_SCSI_T_TMF;
    g_TmfReq.u32Subtype = u32Subtype;
    g_TmfReq.aLun[0] = 1;
    g_TmfReq.aLun[1] = (u8)(u32Lun & 0xff);
    g_TmfReq.u64Tag = u64Tag;

    /*
     * Control descriptor order: TMF req (device-R) + TMF resp (device-W).
     */
    if (virtio_q_add2(&g_qCtrl,
                      (gj_paddr_t)(gj_vaddr_t)&g_TmfReq, (u32)sizeof(g_TmfReq),
                      0,
                      (gj_paddr_t)(gj_vaddr_t)&g_TmfResp,
                      (u32)sizeof(g_TmfResp), 1) < 0) {
        g_u32CtrlFail++;
        return -1;
    }
    virtio_q_kick(&g_qCtrl);
    i32Len = virtio_q_poll(&g_qCtrl, VIRTIO_SCSI_CTRL_SPINS);
    if (i32Len < 0) {
        kprintf("virtio-scsi: ctrl TMF timeout subtype=%u\n", u32Subtype);
        g_u32CtrlFail++;
        return -1;
    }

    g_u32LastResponse = (u32)g_TmfResp.u8Response;
    if (g_TmfResp.u8Response != VIRTIO_SCSI_S_OK &&
        g_TmfResp.u8Response != VIRTIO_SCSI_S_FUNCTION_SUCCEEDED) {
        kprintf("virtio-scsi: ctrl TMF subtype=%u response=%u\n",
                u32Subtype, (unsigned)g_TmfResp.u8Response);
        g_u32CtrlFail++;
        return -1;
    }
    g_u32CtrlOk++;
    return 0;
}

u32
virtio_scsi_ctrl_count(void)
{
    return g_u32CtrlOk;
}

/* ---- Event q1 soft path -------------------------------------------------- */

int
virtio_scsi_event_poll(struct gj_virtio_scsi_event *pOut)
{
    i32 i32Len;
    struct virtio_scsi_event *pEv;

    /* Soft-only / not ready: no async events. */
    if (!g_fReady) {
        return 0;
    }
    if (pOut == NULL) {
        return -1;
    }

    /* Ensure a buffer is posted before soft poll. */
    if (!g_fEventPosted) {
        (void)event_post();
    }
    if (!g_fEventPosted) {
        return 0;
    }

    i32Len = virtio_q_poll(&g_qEvent, VIRTIO_SCSI_EVENT_SPINS);
    if (i32Len < 0) {
        return 0; /* none pending — soft non-blocking */
    }

    g_fEventPosted = 0;
    pEv = (struct virtio_scsi_event *)(void *)g_aEvent;
    pOut->u32Event = pEv->u32Event & ~GJ_VIRTIO_SCSI_EVT_MISSED;
    pOut->u32Missed = (pEv->u32Event & GJ_VIRTIO_SCSI_EVT_MISSED) ? 1u : 0u;
    pOut->u32Reason = pEv->u32Reason;
    pOut->u32Lun = (u32)pEv->aLun[1];
    g_u32EventCount++;

    /* Soft repost so the device can deliver the next event. */
    if (event_post() != 0) {
        kprintf("virtio-scsi: event repost failed\n");
    }

    if (pOut->u32Event != GJ_VIRTIO_SCSI_EVT_NO_EVENT || pOut->u32Missed) {
        kprintf("virtio-scsi: event type=%u reason=%u lun=%u missed=%u\n",
                pOut->u32Event, pOut->u32Reason, pOut->u32Lun, pOut->u32Missed);
    }
    return 1;
}

u32
virtio_scsi_event_count(void)
{
    return g_u32EventCount;
}

/* ---- Soft queue stats ---------------------------------------------------- */

int
virtio_scsi_qstats(struct gj_virtio_scsi_qstats *pOut)
{
    if (pOut == NULL) {
        return -1;
    }
    memset(pOut, 0, sizeof(*pOut));
    pOut->u32Ready = g_fReady ? 1u : 0u;
    pOut->u32Soft = (g_fSoft && !g_fReady) ? 1u : 0u;
    pOut->u32IoOk = g_u32IoCount;
    pOut->u32IoFail = g_u32IoFail;
    pOut->u32CtrlOk = g_u32CtrlOk;
    pOut->u32CtrlFail = g_u32CtrlFail;
    pOut->u32Events = g_u32EventCount;
    pOut->u32EventPosted = g_fEventPosted ? 1u : 0u;
    pOut->u32LastResponse = g_u32LastResponse;
    pOut->u32LastResidual = g_u32LastResidual;
    pOut->u32LastScsiStatus = g_u32LastScsiStatus;
    if (g_fReady) {
        pOut->u16FreeCtrl = virtio_q_num_free(&g_qCtrl);
        pOut->u16FreeEvent = virtio_q_num_free(&g_qEvent);
        pOut->u16FreeReq = virtio_q_num_free(&g_qReq);
    }
    return 0;
}

u16
virtio_scsi_q_free(u32 u32QIdx)
{
    if (!g_fReady) {
        return 0;
    }
    if (u32QIdx == VIRTIO_SCSI_Q_CONTROL) {
        return virtio_q_num_free(&g_qCtrl);
    }
    if (u32QIdx == VIRTIO_SCSI_Q_EVENT) {
        return virtio_q_num_free(&g_qEvent);
    }
    if (u32QIdx == VIRTIO_SCSI_Q_REQUEST) {
        return virtio_q_num_free(&g_qReq);
    }
    return 0;
}
