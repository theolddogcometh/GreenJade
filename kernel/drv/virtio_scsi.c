/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-scsi modern PCI (OASIS virtio-scsi).
 * Pure C, dual license (MIT OR Apache-2.0). No GPL. No Linux virtio source.
 *
 * Queues (fixed OASIS indices):
 *   q0 control — TMF / hotplug (allocated; unused on submit path)
 *   q1 event   — async events (one device-write buffer posted at probe)
 *   q2 request — command I/O (req [+ data-out] → resp [+ data-in])
 *
 * Product markers (serial): "virtio-scsi: ready …"; I/O via scsi_mid/scsi_door.
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
#define VIRTIO_SCSI_Q_SIZE     64u
#define VIRTIO_SCSI_POLL_SPINS 20000000u
#define VIRTIO_SCSI_EVENT_SZ   256u

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

/* Task attribute: SIMPLE (OASIS) */
#define VIRTIO_SCSI_S_SIMPLE 1u

/* Product kind marker from virtio_pci kind_from_device (scsi modern/transitional). */
#define VIRTIO_SCSI_KIND 6u

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

static struct gj_virtio_dev *g_pScsi;
static struct gj_virtq       g_qCtrl;  /* q0 control (reserved) */
static struct gj_virtq       g_qEvent; /* q1 event */
static struct gj_virtq       g_qReq;   /* q2 request0 */
static int                   g_fReady;
static int                   g_fEventPosted;
static u32                   g_u32IoCount;

/* Single outstanding command buffers (identity-mapped BSS). */
static struct virtio_scsi_req_cmd  g_Req __attribute__((aligned(16)));
static struct virtio_scsi_resp_cmd g_Resp __attribute__((aligned(16)));
static u8 g_aData[GJ_VIRTIO_SCSI_DATA_MAX] __attribute__((aligned(16)));
static u8 g_aEvent[VIRTIO_SCSI_EVENT_SZ] __attribute__((aligned(16)));

/*
 * Probe path: find first scsi → modern PCI caps → features → ctrl/event/req qs
 * → post event buffer → DRIVER_OK.
 * Leaves g_fReady=0 and g_pScsi=NULL on any hard failure (no partial ready).
 */
int
virtio_scsi_probe(void)
{
    u32 i;
    u32 c;
    gj_status_t st;

    g_pScsi = NULL;
    g_fReady = 0;
    g_fEventPosted = 0;
    g_u32IoCount = 0;

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
        return -1;
    }

    st = virtio_pci_setup(g_pScsi);
    if (st != GJ_OK || g_pScsi->pCommon == NULL) {
        kprintf("virtio-scsi: pci setup failed %d\n", (int)st);
        g_pScsi = NULL;
        return -1;
    }
    /* Prefer V1; fall back to transitional (empty want mask) */
    st = virtio_negotiate(g_pScsi, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        st = virtio_negotiate(g_pScsi, 0);
        if (st != GJ_OK) {
            kprintf("virtio-scsi: negotiate failed %d\n", (int)st);
            g_pScsi = NULL;
            return -1;
        }
    }

    /* q0 control — required by OASIS layout; TMF path not wired yet */
    st = virtio_q_setup(g_pScsi, &g_qCtrl, (u16)VIRTIO_SCSI_Q_CONTROL,
                        (u16)VIRTIO_SCSI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-scsi: control q failed %d\n", (int)st);
        g_pScsi = NULL;
        return -1;
    }
    /* q1 event — async notifications */
    st = virtio_q_setup(g_pScsi, &g_qEvent, (u16)VIRTIO_SCSI_Q_EVENT,
                        (u16)VIRTIO_SCSI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-scsi: event q failed %d\n", (int)st);
        g_pScsi = NULL;
        return -1;
    }
    /* q2 request0 — command submission */
    st = virtio_q_setup(g_pScsi, &g_qReq, (u16)VIRTIO_SCSI_Q_REQUEST,
                        (u16)VIRTIO_SCSI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-scsi: request q failed %d\n", (int)st);
        g_pScsi = NULL;
        return -1;
    }

    /* Post one event buffer (device-write); I/O still works if this fails */
    memset(g_aEvent, 0, sizeof(g_aEvent));
    if (virtio_q_add(&g_qEvent, (gj_paddr_t)(gj_vaddr_t)g_aEvent,
                     sizeof(g_aEvent), 1) >= 0) {
        g_fEventPosted = 1;
        virtio_q_kick(&g_qEvent);
    } else {
        kprintf("virtio-scsi: initial event post failed\n");
    }

    virtio_set_status(g_pScsi,
                      (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                           GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    g_fReady = 1;
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
virtio_scsi_submit(struct gj_scsi_request *pReq)
{
    i32 i32Len;
    u32 cbData;

    if (!g_fReady || pReq == NULL || pReq->cdb.u8CdbLen == 0 ||
        pReq->cdb.u8CdbLen > 32) {
        return -1;
    }

    memset(&g_Req, 0, sizeof(g_Req));
    memset(&g_Resp, 0xff, sizeof(g_Resp)); /* poison; device overwrites */
    /*
     * Single-level LUN addressing (virtio-scsi): byte0=1, byte1=LUN.
     */
    g_Req.aLun[0] = 1;
    g_Req.aLun[1] = (u8)(pReq->u32Lun & 0xff);
    g_Req.u64Id = (u64)g_u32IoCount + 1ull;
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
            return -1;
        }
    } else if (pReq->fDataIn) {
        if (virtio_q_add3(&g_qReq,
                          (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                          (gj_paddr_t)(gj_vaddr_t)&g_Resp, (u32)sizeof(g_Resp),
                          1,
                          (gj_paddr_t)(gj_vaddr_t)g_aData, cbData, 1) < 0) {
            return -1;
        }
    } else {
        if (virtio_q_add3(&g_qReq,
                          (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                          (gj_paddr_t)(gj_vaddr_t)g_aData, cbData, 0,
                          (gj_paddr_t)(gj_vaddr_t)&g_Resp, (u32)sizeof(g_Resp),
                          1) < 0) {
            return -1;
        }
    }
    virtio_q_kick(&g_qReq);
    i32Len = virtio_q_poll(&g_qReq, VIRTIO_SCSI_POLL_SPINS);
    if (i32Len < 0) {
        kprintf("virtio-scsi: timeout\n");
        return -1;
    }
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
        memcpy(pReq->pData, g_aData, cbData);
    }
    g_u32IoCount++;
    if (g_Resp.u8Response != VIRTIO_SCSI_S_OK) {
        kprintf("virtio-scsi: response=%u status=%u sense_len=%u residual=%u\n",
                (unsigned)g_Resp.u8Response, (unsigned)g_Resp.u8Status,
                g_Resp.u32SenseLen, g_Resp.u32Residual);
        return -1;
    }
    if (g_Resp.u8Status != GJ_SCSI_GOOD) {
        kprintf("virtio-scsi: scsi status=%u\n", (unsigned)g_Resp.u8Status);
        return -1;
    }
    return 0;
}

u32
virtio_scsi_io_count(void)
{
    return g_u32IoCount;
}
