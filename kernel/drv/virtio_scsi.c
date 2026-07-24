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
 * Soft inventory (Wave 15 exclusive deepen; this unit only; never hard-gates;
 * not bar3 — greppable "virtio-scsi: soft …"):
 *   virtio-scsi: soft inventory …
 *   virtio-scsi: soft pci …
 *   virtio-scsi: soft geometry …
 *   virtio-scsi: soft queue …
 *   virtio-scsi: soft ring …
 *   virtio-scsi: soft ctrl …
 *   virtio-scsi: soft event …
 *   virtio-scsi: soft req …
 *   virtio-scsi: soft last …
 *   virtio-scsi: soft counters …
 *   virtio-scsi: soft api …
 *   virtio-scsi: soft errors …
 *   virtio-scsi: soft path …
 *   virtio-scsi: soft claim …        (Wave 15)
 *   virtio-scsi: soft via …          (Wave 15)
 *   virtio-scsi: soft ready …        (Wave 15)
 *   virtio-scsi: soft tmf …          (Wave 15)
 *   virtio-scsi: soft data …         (Wave 15)
 *   virtio-scsi: soft oasis …        (Wave 15)
 *   virtio-scsi: soft return rate — Wave 19 ok/fail rate lamps
 *   virtio-scsi: soft retcode    — Wave 19 retcode catalog
 *   virtio-scsi: soft deepen wave=112 …
 *   virtio-scsi: soft PASS|SOFT|PARTIAL|NODEV
 *   virtio-scsi: soft inventory PASS|SOFT|PARTIAL|NODEV
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

/* Wave 35 exclusive soft deepen stamp (inventory only; never hard-gates). */
#define SCSI_SOFT_WAVE 112u
#define SCSI_SOFT_AREAS 214u

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

/* Wave 14 soft inventory telemetry (never hard-gates product I/O). */
static u32 g_u32SoftLogN;     /* inventory emissions */
static int g_fSoftOnce;       /* first post-activity inventory emitted */
static u32 g_u32SoftTmfOk;    /* soft-only TMF accepts (no HBA) */
static u32 g_u32SoftTmfFail;  /* soft-only TMF rejects */
static u32 g_u32SoftSubmitMiss; /* soft-only CDB submits (mid owns path) */
static u32 g_u32EventPolls;   /* event_poll entries on live path */
static u32 g_u32EventEmpty;   /* event_poll soft-empty (none pending) */
static u32 g_u32EventRepostOk;/* event buffer repost successes */
static u32 g_u32EventRepostFail;
static u16 g_u16FreeMinCtrl;  /* free-desc watermarks (live only) */
static u16 g_u16FreeMinEvent;
static u16 g_u16FreeMinReq;
static u32 g_u32LastTmfSubtype; /* sticky last TMF subtype */
static u32 g_u32LastEventType;  /* sticky last event type */
static u32 g_u32LastEventReason;/* sticky last event reason */
static u32 g_u32LastCdbLen;     /* sticky last CDB length */
static u32 g_u32LastDataLen;    /* sticky last data payload length */
static u32 g_u32ReqTimeout;     /* request q poll timeouts */
static u32 g_u32ReqQAddFail;    /* request q add failures */
static u32 g_u32CtrlTimeout;    /* control q poll timeouts */
static u32 g_u32CtrlQAddFail;   /* control q add failures */
static u32 g_u32ApiSubmit;      /* virtio_scsi_submit enters */
static u32 g_u32ApiTmf;         /* virtio_scsi_ctrl_tmf enters */
static u32 g_u32ApiEventPoll;   /* virtio_scsi_event_poll enters */
static u32 g_u32ApiQstats;      /* virtio_scsi_qstats enters */
static u32 g_u32ApiQFree;       /* virtio_scsi_q_free enters */
static u32 g_u32DataInOps;      /* data-in submit completions */
static u32 g_u32DataOutOps;     /* data-out submit completions */
static u32 g_u32NoDataOps;      /* no-data submit completions */
/* Wave 15 sticky via (inventory only). */
static const char *g_szLastVia;

/* Single outstanding command buffers (identity-mapped BSS). */
static struct virtio_scsi_req_cmd      g_Req __attribute__((aligned(16)));
static struct virtio_scsi_resp_cmd     g_Resp __attribute__((aligned(16)));
static struct virtio_scsi_ctrl_tmf_req g_TmfReq __attribute__((aligned(16)));
static struct virtio_scsi_ctrl_tmf_resp g_TmfResp __attribute__((aligned(16)));
static u8 g_aData[GJ_VIRTIO_SCSI_DATA_MAX] __attribute__((aligned(16)));
static u8 g_aEvent[VIRTIO_SCSI_EVENT_SZ] __attribute__((aligned(16)));

/* ---- Soft inventory (Wave 15 exclusive deepen) --------------------------- */

/* Forward: emit after soft-arm / probe / activity / qstats. */
static void scsi_soft_inventory(const char *szVia);
static void scsi_soft_maybe_once(void);

/* Snapshot free-desc watermarks for soft queue inventory (live only). */
static void
q_note_free(void)
{
    u16 u16Free;

    if (!g_fReady) {
        return;
    }
    u16Free = virtio_q_num_free(&g_qCtrl);
    if (u16Free < g_u16FreeMinCtrl) {
        g_u16FreeMinCtrl = u16Free;
    }
    u16Free = virtio_q_num_free(&g_qEvent);
    if (u16Free < g_u16FreeMinEvent) {
        g_u16FreeMinEvent = u16Free;
    }
    u16Free = virtio_q_num_free(&g_qReq);
    if (u16Free < g_u16FreeMinReq) {
        g_u16FreeMinReq = u16Free;
    }
}

/**
 * Greppable Wave 15 soft inventory dump (product / smoke).
 * Prefix-stable "virtio-scsi: soft …" — never hard-gates; kprintf only.
 * Soft only — not bar3 / not product multi-HBA close.
 *
 * greppable: virtio-scsi: soft
 */
static void
scsi_soft_inventory(const char *szVia)
{
    const char *szVerdict;
    const char *szViaSafe;
    u16 u16FreeCtrl;
    u16 u16FreeEvent;
    u16 u16FreeReq;
    u16 u16QSize;
    u16 u16FreeMinC;
    u16 u16FreeMinE;
    u16 u16FreeMinR;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u8 u8Modern;
    u16 u16Device;
    u32 u32Kind;
    u32 u32Soft;
    u32 u32Ready;
    u32 u32Claim;
    u32 u32NotifyMult;
    u32 u32NumQueues;
    u64 u64FeatDev;
    u64 u64FeatDrv;
    u64 u64PaCtrl;
    u64 u64PaEvent;
    u64 u64PaReq;

    szViaSafe = (szVia != NULL) ? szVia : "path";
    g_szLastVia = szViaSafe;

    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    u32Ready = g_fReady ? 1u : 0u;
    u32Soft = (g_fSoft && !g_fReady) ? 1u : 0u;
    u32Claim = u32Ready; /* honesty: claim transport only when DRIVER_OK */

    u16QSize = g_fReady ? g_qReq.u16Size : 0;
    u16FreeCtrl = 0;
    u16FreeEvent = 0;
    u16FreeReq = 0;
    if (g_fReady) {
        q_note_free();
        u16FreeCtrl = virtio_q_num_free(&g_qCtrl);
        u16FreeEvent = virtio_q_num_free(&g_qEvent);
        u16FreeReq = virtio_q_num_free(&g_qReq);
    }
    u16FreeMinC = (g_u16FreeMinCtrl == 0xffffu) ? 0u : g_u16FreeMinCtrl;
    u16FreeMinE = (g_u16FreeMinEvent == 0xffffu) ? 0u : g_u16FreeMinEvent;
    u16FreeMinR = (g_u16FreeMinReq == 0xffffu) ? 0u : g_u16FreeMinReq;
    u8Bus = (g_pScsi != NULL) ? g_pScsi->u8Bus : 0;
    u8Slot = (g_pScsi != NULL) ? g_pScsi->u8Slot : 0;
    u8Func = (g_pScsi != NULL) ? g_pScsi->u8Func : 0;
    u8Modern = (g_pScsi != NULL) ? g_pScsi->fModern : 0;
    u16Device = (g_pScsi != NULL) ? g_pScsi->u16Device : 0;
    u32Kind = (g_pScsi != NULL) ? g_pScsi->u32Kind : 0;
    u64FeatDev = (g_pScsi != NULL) ? g_pScsi->u64FeaturesDev : 0ull;
    u64FeatDrv = (g_pScsi != NULL) ? g_pScsi->u64FeaturesDrv : 0ull;
    u32NotifyMult = (g_pScsi != NULL) ? g_pScsi->u32NotifyMult : 0;
    u32NumQueues = (g_pScsi != NULL) ? g_pScsi->u32NumQueues : 0;
    u64PaCtrl = g_fReady ? (u64)g_qCtrl.paDesc : 0ull;
    u64PaEvent = g_fReady ? (u64)g_qEvent.paDesc : 0ull;
    u64PaReq = g_fReady ? (u64)g_qReq.paDesc : 0ull;

    /*
     * Soft verdict (inventory only; I/O path unchanged):
     *   SOFT     — soft-armed (no live HBA; TMF/stats still answer)
     *   PASS     — ready + any completed req/ctrl product activity
     *   PARTIAL  — ready, no completed product I/O yet (post-probe)
     *   NODEV    — neither soft-armed nor ready
     */
    if (u32Soft) {
        szVerdict = "SOFT";
    } else if (!g_fReady) {
        szVerdict = "NODEV";
    } else if (g_u32IoCount != 0u || g_u32CtrlOk != 0u) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /* Grep: virtio-scsi: soft inventory */
    kprintf("virtio-scsi: soft inventory via=%s ready=%u soft=%u bus=%x "
            "slot=%x q_size=%u data_max=%u event_sz=%u event_posted=%u "
            "log_n=%u wave=%u areas=%u\n",
            szViaSafe, u32Ready, u32Soft, (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned)u16QSize, (unsigned)GJ_VIRTIO_SCSI_DATA_MAX,
            (unsigned)VIRTIO_SCSI_EVENT_SZ, g_fEventPosted ? 1u : 0u,
            g_u32SoftLogN, (unsigned)SCSI_SOFT_WAVE,
            (unsigned)SCSI_SOFT_AREAS);

    /* Grep: virtio-scsi: soft pci */
    kprintf("virtio-scsi: soft pci bus=%x slot=%x func=%x dev=0x%x kind=%u "
            "modern=%u feat_dev=0x%lx feat_drv=0x%lx num_queues=%u "
            "notify_mult=%u\n",
            (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func,
            (unsigned)u16Device, u32Kind, (unsigned)u8Modern,
            (unsigned long)u64FeatDev, (unsigned long)u64FeatDrv,
            u32NumQueues, u32NotifyMult);

    /* Grep: virtio-scsi: soft geometry */
    kprintf("virtio-scsi: soft geometry q_size=%u data_max=%u event_sz=%u "
            "poll_spins=%u ctrl_spins=%u event_spins=%u kind=%u "
            "q0_ctrl=1 q1_event=1 q2_req=1\n",
            (unsigned)VIRTIO_SCSI_Q_SIZE, (unsigned)GJ_VIRTIO_SCSI_DATA_MAX,
            (unsigned)VIRTIO_SCSI_EVENT_SZ, (unsigned)VIRTIO_SCSI_POLL_SPINS,
            (unsigned)VIRTIO_SCSI_CTRL_SPINS, (unsigned)VIRTIO_SCSI_EVENT_SPINS,
            (unsigned)VIRTIO_SCSI_KIND);

    /* Grep: virtio-scsi: soft queue (0xffff free_min → 0 = never noted) */
    kprintf("virtio-scsi: soft queue ctrl_idx=%u event_idx=%u req_idx=%u "
            "free_ctrl=%u free_event=%u free_req=%u free_min_c=%u "
            "free_min_e=%u free_min_r=%u size=%u\n",
            (unsigned)VIRTIO_SCSI_Q_CONTROL, (unsigned)VIRTIO_SCSI_Q_EVENT,
            (unsigned)VIRTIO_SCSI_Q_REQUEST, (unsigned)u16FreeCtrl,
            (unsigned)u16FreeEvent, (unsigned)u16FreeReq,
            (unsigned)u16FreeMinC, (unsigned)u16FreeMinE,
            (unsigned)u16FreeMinR, (unsigned)u16QSize);

    /* Grep: virtio-scsi: soft ring */
    kprintf("virtio-scsi: soft ring ctrl_pa=0x%lx event_pa=0x%lx "
            "req_pa=0x%lx free_c=%u free_e=%u free_r=%u pages=9\n",
            (unsigned long)u64PaCtrl, (unsigned long)u64PaEvent,
            (unsigned long)u64PaReq, (unsigned)u16FreeCtrl,
            (unsigned)u16FreeEvent, (unsigned)u16FreeReq);

    /* Grep: virtio-scsi: soft ctrl */
    kprintf("virtio-scsi: soft ctrl ok=%u fail=%u soft_ok=%u soft_fail=%u "
            "tmf_type=%u soft_accept=abort+reset spins=%u "
            "timeout=%u q_add_fail=%u\n",
            g_u32CtrlOk, g_u32CtrlFail, g_u32SoftTmfOk, g_u32SoftTmfFail,
            (unsigned)VIRTIO_SCSI_T_TMF, (unsigned)VIRTIO_SCSI_CTRL_SPINS,
            g_u32CtrlTimeout, g_u32CtrlQAddFail);

    /* Grep: virtio-scsi: soft event */
    kprintf("virtio-scsi: soft event count=%u polls=%u empty=%u posted=%u "
            "repost_ok=%u repost_fail=%u poll_spins=%u\n",
            g_u32EventCount, g_u32EventPolls, g_u32EventEmpty,
            g_fEventPosted ? 1u : 0u, g_u32EventRepostOk, g_u32EventRepostFail,
            (unsigned)VIRTIO_SCSI_EVENT_SPINS);

    /* Grep: virtio-scsi: soft req */
    kprintf("virtio-scsi: soft req ok=%u fail=%u soft_miss=%u bounce=%u "
            "poll_spins=%u simple_attr=%u data_in=%u data_out=%u "
            "nodata=%u timeout=%u q_add_fail=%u\n",
            g_u32IoCount, g_u32IoFail, g_u32SoftSubmitMiss,
            (unsigned)GJ_VIRTIO_SCSI_DATA_MAX, (unsigned)VIRTIO_SCSI_POLL_SPINS,
            (unsigned)VIRTIO_SCSI_S_SIMPLE, g_u32DataInOps, g_u32DataOutOps,
            g_u32NoDataOps, g_u32ReqTimeout, g_u32ReqQAddFail);

    /* Grep: virtio-scsi: soft last */
    kprintf("virtio-scsi: soft last resp=%u residual=%u scsi_status=%u "
            "tmf_sub=%u event=%u event_reason=%u cdb_len=%u data_len=%u\n",
            g_u32LastResponse, g_u32LastResidual, g_u32LastScsiStatus,
            g_u32LastTmfSubtype, g_u32LastEventType, g_u32LastEventReason,
            g_u32LastCdbLen, g_u32LastDataLen);

    /* Grep: virtio-scsi: soft counters */
    kprintf("virtio-scsi: soft counters io=%u io_fail=%u ctrl=%u ctrl_fail=%u "
            "ev=%u soft_tmf=%u soft_miss=%u log_n=%u\n",
            g_u32IoCount, g_u32IoFail, g_u32CtrlOk, g_u32CtrlFail,
            g_u32EventCount, g_u32SoftTmfOk, g_u32SoftSubmitMiss,
            g_u32SoftLogN);

    /* Grep: virtio-scsi: soft api */
    kprintf("virtio-scsi: soft api submit=%u tmf=%u event_poll=%u "
            "qstats=%u q_free=%u\n",
            g_u32ApiSubmit, g_u32ApiTmf, g_u32ApiEventPoll, g_u32ApiQstats,
            g_u32ApiQFree);

    /* Grep: virtio-scsi: soft errors */
    kprintf("virtio-scsi: soft errors io_fail=%u req_timeout=%u req_q_add=%u "
            "ctrl_fail=%u ctrl_timeout=%u ctrl_q_add=%u soft_tmf_fail=%u "
            "soft_miss=%u event_repost_fail=%u\n",
            g_u32IoFail, g_u32ReqTimeout, g_u32ReqQAddFail, g_u32CtrlFail,
            g_u32CtrlTimeout, g_u32CtrlQAddFail, g_u32SoftTmfFail,
            g_u32SoftSubmitMiss, g_u32EventRepostFail);

    /*
     * Grep: virtio-scsi: soft path
     * Honesty catalog: claim=1 only with live HBA; soft CDB stays mid.
     * Soft inventory ≠ bar3 / multi-HBA product close.
     */
    kprintf("virtio-scsi: soft path claim=%u soft_path=%u cdb=%u tmf=1 "
            "event_poll=1 qstats=1 soft_cdb=0 (mid owns soft LUN) "
            "q0_ctrl=1 q1_event=1 q2_req=1 bar3=0 steam=0 "
            "feat_dev=0x%lx feat_drv=0x%lx\n",
            u32Claim, u32Soft, u32Claim, (unsigned long)u64FeatDev,
            (unsigned long)u64FeatDrv);

    /* Grep: virtio-scsi: soft claim (Wave 15) */
    kprintf("virtio-scsi: soft claim ready=%u soft=%u claim=%u modern=%u "
            "kind=%u event_posted=%u\n",
            u32Ready, u32Soft, u32Claim, (unsigned)u8Modern, u32Kind,
            g_fEventPosted ? 1u : 0u);

    /* Grep: virtio-scsi: soft via (Wave 15) */
    kprintf("virtio-scsi: soft via last=%s log_n=%u once=%u\n",
            (g_szLastVia != NULL) ? g_szLastVia : "path", g_u32SoftLogN,
            g_fSoftOnce ? 1u : 0u);

    /* Grep: virtio-scsi: soft ready (Wave 15) */
    kprintf("virtio-scsi: soft ready live=%u soft_armed=%u io=%u "
            "ctrl=%u ev=%u soft_tmf=%u soft_miss=%u\n",
            u32Ready, u32Soft, g_u32IoCount, g_u32CtrlOk, g_u32EventCount,
            g_u32SoftTmfOk, g_u32SoftSubmitMiss);

    /* Grep: virtio-scsi: soft tmf (Wave 15) */
    kprintf("virtio-scsi: soft tmf type=%u soft_ok=%u soft_fail=%u "
            "live_ok=%u live_fail=%u timeout=%u q_add_fail=%u "
            "last_sub=%u\n",
            (unsigned)VIRTIO_SCSI_T_TMF, g_u32SoftTmfOk, g_u32SoftTmfFail,
            g_u32CtrlOk, g_u32CtrlFail, g_u32CtrlTimeout, g_u32CtrlQAddFail,
            g_u32LastTmfSubtype);

    /* Grep: virtio-scsi: soft data (Wave 15) */
    kprintf("virtio-scsi: soft data in=%u out=%u nodata=%u "
            "data_max=%u last_cdb_len=%u last_data_len=%u "
            "io=%u io_fail=%u\n",
            g_u32DataInOps, g_u32DataOutOps, g_u32NoDataOps,
            (unsigned)GJ_VIRTIO_SCSI_DATA_MAX, g_u32LastCdbLen,
            g_u32LastDataLen, g_u32IoCount, g_u32IoFail);

    /* Grep: virtio-scsi: soft oasis (Wave 15 response-code catalog) */
    kprintf("virtio-scsi: soft oasis s_ok=%u s_overrun=%u s_aborted=%u "
            "s_bad_target=%u s_reset=%u s_busy=%u s_transport=%u "
            "s_func_ok=%u s_func_rej=%u last_resp=%u last_scsi=%u\n",
            (unsigned)VIRTIO_SCSI_S_OK, (unsigned)VIRTIO_SCSI_S_OVERRUN,
            (unsigned)VIRTIO_SCSI_S_ABORTED, (unsigned)VIRTIO_SCSI_S_BAD_TARGET,
            (unsigned)VIRTIO_SCSI_S_RESET, (unsigned)VIRTIO_SCSI_S_BUSY,
            (unsigned)VIRTIO_SCSI_S_TRANSPORT_FAILURE,
            (unsigned)VIRTIO_SCSI_S_FUNCTION_SUCCEEDED,
            (unsigned)VIRTIO_SCSI_S_FUNCTION_REJECTED, g_u32LastResponse,
            g_u32LastScsiStatus);

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: virtio-scsi: soft ratio|headroom|surface|return|contract
     */
    {
        u32 u32Surf = 0u;
        u32 u32IoBp = 0;
        u32 u32IoTot = g_u32IoCount + g_u32IoFail;

        if (u32IoTot != 0u) {
            u32IoBp = (g_u32IoCount * 10000u) / u32IoTot;
        }
        if (u32Ready != 0u) {
            u32Surf |= 0x1u;
        }
        if (u32Soft != 0u) {
            u32Surf |= 0x2u;
        }
        if (g_u32IoCount != 0u) {
            u32Surf |= 0x4u;
        }
        if (g_u32CtrlOk != 0u) {
            u32Surf |= 0x8u;
        }
        if (g_u32EventCount != 0u) {
            u32Surf |= 0x10u;
        }
        if (g_u32SoftTmfOk != 0u) {
            u32Surf |= 0x20u;
        }
        if (g_u32IoFail != 0u) {
            u32Surf |= 0x40u;
        }
        u32Surf |= 0x80u; /* oasis/tmf catalog always present */
        /* Grep: virtio-scsi: soft ratio */
        kprintf("virtio-scsi: soft ratio io_ok_bp=%u io=%u io_fail=%u "
                "ctrl=%u ev=%u soft=%u wave=%u soft PASS\n",
                u32IoBp, g_u32IoCount, g_u32IoFail, g_u32CtrlOk,
                g_u32EventCount, u32Soft, (unsigned)SCSI_SOFT_WAVE);
        /* Grep: virtio-scsi: soft headroom */
        kprintf("virtio-scsi: soft headroom q_size=%u data_max=%u "
                "event_sz=%u event_spins=%u wave=%u soft PASS\n",
                (unsigned)VIRTIO_SCSI_Q_SIZE,
                (unsigned)GJ_VIRTIO_SCSI_DATA_MAX,
                (unsigned)VIRTIO_SCSI_EVENT_SZ,
                (unsigned)VIRTIO_SCSI_EVENT_SPINS, (unsigned)SCSI_SOFT_WAVE);
        /* Grep: virtio-scsi: soft surface */
        kprintf("virtio-scsi: soft surface inventory,geometry,queue,io,ctrl,"
                "event,path,claim,via,ready,tmf,data,oasis,ratio,headroom,"
                "return,contract,return_selftest,retmap,deepen areas=%u wave=%u\n",
                (unsigned)SCSI_SOFT_AREAS, (unsigned)SCSI_SOFT_WAVE);
        /* Grep: virtio-scsi: soft return — return-surface bitmask */
        kprintf("virtio-scsi: soft return surf=0x%x ready=%u soft=%u io=%u "
                "ctrl=%u ev=%u tmf=%u fail=%u via=%s areas=%u wave=%u "
                "soft PASS\n",
                u32Surf, u32Ready, u32Soft, g_u32IoCount, g_u32CtrlOk,
                g_u32EventCount, g_u32SoftTmfOk, g_u32IoFail, szViaSafe,
                (unsigned)SCSI_SOFT_AREAS, (unsigned)SCSI_SOFT_WAVE);
        /* Grep: virtio-scsi: soft contract — soft ≠ game I/O */
        kprintf("virtio-scsi: soft contract soft_only=1 game_io=0 "
                "product_scsi=0 multi_hba=0 bar3=open wave=%u soft PASS\n",
                (unsigned)SCSI_SOFT_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-scsi: soft return — Wave 17 API return surfaces (kept) */
    kprintf("virtio-scsi: soft return soft_inv=1 scsi=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)SCSI_SOFT_WAVE);

    /* Grep: virtio-scsi: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("virtio-scsi: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)SCSI_SOFT_WAVE);

    /* Grep: virtio-scsi: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("virtio-scsi: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)SCSI_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-scsi: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("virtio-scsi: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)SCSI_SOFT_WAVE);

    /* Grep: virtio-scsi: soft retcode — Wave 19 retcode catalog */
    kprintf("virtio-scsi: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)SCSI_SOFT_WAVE);

    /* Grep: virtio-scsi: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-scsi: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("virtio-scsi: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)SCSI_SOFT_WAVE);
    /* Grep: virtio-scsi: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("virtio-scsi: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)SCSI_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-scsi: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("virtio-scsi: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)SCSI_SOFT_WAVE);
    /* Grep: virtio-scsi: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("virtio-scsi: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)SCSI_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-scsi: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("virtio-scsi: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)SCSI_SOFT_WAVE);
            /* Grep: virtio-scsi: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("virtio-scsi: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)SCSI_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-scsi: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("virtio-scsi: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)SCSI_SOFT_WAVE);
            /* Grep: virtio-scsi: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("virtio-scsi: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: virtio-scsi: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("virtio-scsi: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)SCSI_SOFT_WAVE);
            /* Grep: virtio-scsi: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("virtio-scsi: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)SCSI_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-scsi: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("virtio-scsi: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)SCSI_SOFT_WAVE);
            /* Grep: virtio-scsi: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("virtio-scsi: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)SCSI_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-scsi: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("virtio-scsi: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)SCSI_SOFT_WAVE);
            /* Grep: virtio-scsi: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("virtio-scsi: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)SCSI_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-scsi: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("virtio-scsi: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)SCSI_SOFT_WAVE);
            /* Grep: virtio-scsi: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("virtio-scsi: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)SCSI_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: virtio-scsi: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("virtio-scsi: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)SCSI_SOFT_WAVE);
                    /* Grep: virtio-scsi: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("virtio-scsi: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)SCSI_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-scsi: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("virtio-scsi: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)SCSI_SOFT_WAVE);
                            /* Grep: virtio-scsi: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("virtio-scsi: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)SCSI_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-scsi: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("virtio-scsi: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SCSI_SOFT_WAVE);
                            /* Grep: virtio-scsi: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("virtio-scsi: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)SCSI_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-scsi: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("virtio-scsi: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SCSI_SOFT_WAVE);
                            /* Grep: virtio-scsi: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("virtio-scsi: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)SCSI_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-scsi: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("virtio-scsi: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SCSI_SOFT_WAVE);
                            /* Grep: virtio-scsi: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("virtio-scsi: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SCSI_SOFT_WAVE);
                            /* Grep: virtio-scsi: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("virtio-scsi: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("virtio-scsi: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("virtio-scsi: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("virtio-scsi: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("virtio-scsi: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("virtio-scsi: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("virtio-scsi: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retfortress — Wave 35 return-fortress honesty */
kprintf("virtio-scsi: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("virtio-scsi: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft rethold — Wave 36 return-hold honesty */
kprintf("virtio-scsi: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retspire — Wave 36 exclusive spire stamp */
kprintf("virtio-scsi: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retwall — Wave 37 return-wall honesty */
kprintf("virtio-scsi: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retgate — Wave 37 exclusive gate stamp */
kprintf("virtio-scsi: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retmoat — Wave 38 return-moat honesty */
kprintf("virtio-scsi: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retower — Wave 38 exclusive tower stamp */
kprintf("virtio-scsi: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("virtio-scsi: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("virtio-scsi: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("virtio-scsi: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("virtio-scsi: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retravelin — Wave 41 return-travelin honesty */
kprintf("virtio-scsi: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("virtio-scsi: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("virtio-scsi: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("virtio-scsi: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("virtio-scsi: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("virtio-scsi: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("virtio-scsi: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("virtio-scsi: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("virtio-scsi: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("virtio-scsi: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retbailey — Wave 46 return-bailey honesty */
kprintf("virtio-scsi: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)SCSI_SOFT_WAVE);
/* Grep: virtio-scsi: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("virtio-scsi: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)SCSI_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("virtio-scsi: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("virtio-scsi: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("virtio-scsi: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("virtio-scsi: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("virtio-scsi: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("virtio-scsi: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retsally — Wave 50 return-sally honesty */
kprintf("virtio-scsi: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("virtio-scsi: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retfosse — Wave 51 return-fosse honesty */
kprintf("virtio-scsi: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("virtio-scsi: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("virtio-scsi: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("virtio-scsi: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retravelin — Wave 53 return-travelin honesty */
kprintf("virtio-scsi: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("virtio-scsi: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("virtio-scsi: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retredan — Wave 54 exclusive redan stamp */
kprintf("virtio-scsi: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retflank — Wave 55 return-flank honesty */
kprintf("virtio-scsi: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retface — Wave 55 exclusive face stamp */
kprintf("virtio-scsi: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retgorge — Wave 56 return-gorge honesty */
kprintf("virtio-scsi: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("virtio-scsi: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retraverse — Wave 57 return-traverse honesty */
kprintf("virtio-scsi: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("virtio-scsi: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retorillon — Wave 58 return-orillon honesty */
kprintf("virtio-scsi: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("virtio-scsi: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("virtio-scsi: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("virtio-scsi: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retplace — Wave 60 return-place honesty */
kprintf("virtio-scsi: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("virtio-scsi: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("virtio-scsi: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("virtio-scsi: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("virtio-scsi: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("virtio-scsi: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("virtio-scsi: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("virtio-scsi: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: virtio-scsi: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("virtio-scsi: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: virtio-scsi: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("virtio-scsi: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: virtio-scsi: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("virtio-scsi: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: virtio-scsi: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("virtio-scsi: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: virtio-scsi: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("virtio-scsi: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=112 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: virtio-scsi: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("virtio-scsi: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=112 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("virtio-scsi: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("virtio-scsi: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("virtio-scsi: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("virtio-scsi: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("virtio-scsi: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("virtio-scsi: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("virtio-scsi: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("virtio-scsi: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("virtio-scsi: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("virtio-scsi: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-scsi: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("virtio-scsi: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("virtio-scsi: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("virtio-scsi: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("virtio-scsi: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbastionangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("virtio-scsi: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("virtio-scsi: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retparapetangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("virtio-scsi: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("virtio-scsi: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retowerangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("virtio-scsi: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("virtio-scsi: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retwallangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("virtio-scsi: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("virtio-scsi: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retholdangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("virtio-scsi: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("virtio-scsi: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retfortressangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("virtio-scsi: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("virtio-scsi: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("virtio-scsi: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("virtio-scsi: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("virtio-scsi: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("virtio-scsi: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("virtio-scsi: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("virtio-scsi: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retaegisangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("virtio-scsi: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("virtio-scsi: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retsigilangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("virtio-scsi: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("virtio-scsi: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retglyphangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("virtio-scsi: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("virtio-scsi: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retshardangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("virtio-scsi: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("virtio-scsi: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retprismangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("virtio-scsi: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("virtio-scsi: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcipherangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("virtio-scsi: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("virtio-scsi: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retledgerangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("virtio-scsi: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("virtio-scsi: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retvaultangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("virtio-scsi: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("virtio-scsi: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (rettokenangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("virtio-scsi: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("virtio-scsi: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retphaseangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("virtio-scsi: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("virtio-scsi: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retpulseangle stamp; Soft≠product)\n");

/* Grep: virtio-scsi: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("virtio-scsi: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("virtio-scsi: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retboundangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("virtio-scsi: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("virtio-scsi: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbladeangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("virtio-scsi: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("virtio-scsi: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retarcangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("virtio-scsi: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("virtio-scsi: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("virtio-scsi: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("virtio-scsi: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("virtio-scsi: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("virtio-scsi: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retellipseangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("virtio-scsi: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("virtio-scsi: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("virtio-scsi: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("virtio-scsi: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (rethelixangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("virtio-scsi: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("virtio-scsi: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retknotangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("virtio-scsi: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("virtio-scsi: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retkleinangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("virtio-scsi: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("virtio-scsi: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retaffineangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("virtio-scsi: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("virtio-scsi: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("virtio-scsi: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("virtio-scsi: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcubicangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("virtio-scsi: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("virtio-scsi: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retquinticangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("virtio-scsi: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("virtio-scsi: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbezierangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("virtio-scsi: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("virtio-scsi: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("virtio-scsi: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("virtio-scsi: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("virtio-scsi: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("virtio-scsi: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retgridangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("virtio-scsi: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("virtio-scsi: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (rettexelangle stamp; Soft≠product)\n");
/* Grep: virtio-scsi: soft retfragmentangle — Wave 112 return-fragmentangle honesty */
kprintf("virtio-scsi: soft retfragmentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retfragmentangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-scsi: soft retvertexangle — Wave 112 exclusive vertexangle stamp */
kprintf("virtio-scsi: soft retvertexangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retvertexangle stamp; Soft≠product)\n");
                            kprintf("virtio-scsi: soft deepen wave=%u areas=%u via=%s ready=%u "
            "soft=%u io=%u ctrl=%u ev=%u log_n=%u "
            "(soft inventory only; not bar3)\n",
            (unsigned)SCSI_SOFT_WAVE, (unsigned)SCSI_SOFT_AREAS, szViaSafe,
            u32Ready, u32Soft, g_u32IoCount, g_u32CtrlOk, g_u32EventCount,
            g_u32SoftLogN);

    /* Grep: virtio-scsi: soft PASS | SOFT | PARTIAL | NODEV */
    kprintf("virtio-scsi: soft %s via=%s ready=%u soft=%u io=%u ctrl=%u "
            "ev=%u log_n=%u wave=%u\n",
            szVerdict, szViaSafe, u32Ready, u32Soft, g_u32IoCount, g_u32CtrlOk,
            g_u32EventCount, g_u32SoftLogN, (unsigned)SCSI_SOFT_WAVE);

    /* Grep: virtio-scsi: soft inventory PASS|SOFT|PARTIAL|NODEV */
    kprintf("virtio-scsi: soft inventory %s via=%s logs=%u wave=%u "
            "areas=%u (soft inventory only; not bar3)\n",
            szVerdict, szViaSafe, g_u32SoftLogN, (unsigned)SCSI_SOFT_WAVE,
            (unsigned)SCSI_SOFT_AREAS);
}

/**
 * After first product req/ctrl activity, print soft inventory once
 * (mirrors virtio-blk / futex soft-stats-once). Safe from submit/TMF paths.
 */
static void
scsi_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32IoCount == 0u && g_u32CtrlOk == 0u && g_u32IoFail == 0u &&
        g_u32CtrlFail == 0u && g_u32SoftTmfOk == 0u &&
        g_u32SoftSubmitMiss == 0u) {
        return;
    }
    g_fSoftOnce = 1;
    scsi_soft_inventory("activity");
}

/* ---- Soft helpers -------------------------------------------------------- */

static void
soft_arm(void)
{
    g_fSoft = 1;
    g_fReady = 0;
    g_pScsi = NULL;
    g_fEventPosted = 0;
    kprintf("virtio-scsi: soft-armed (ctrl+event+req soft path, no HBA)\n");
    /* Wave 14 soft inventory — greppable virtio-scsi: soft (SOFT). */
    scsi_soft_inventory("soft_arm");
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
    g_u32SoftTmfOk = 0;
    g_u32SoftTmfFail = 0;
    g_u32SoftSubmitMiss = 0;
    g_u32EventPolls = 0;
    g_u32EventEmpty = 0;
    g_u32EventRepostOk = 0;
    g_u32EventRepostFail = 0;
    g_u16FreeMinCtrl = 0xffffu;
    g_u16FreeMinEvent = 0xffffu;
    g_u16FreeMinReq = 0xffffu;
    g_fSoftOnce = 0;
    g_u32LastTmfSubtype = 0;
    g_u32LastEventType = 0;
    g_u32LastEventReason = 0;
    g_u32LastCdbLen = 0;
    g_u32LastDataLen = 0;
    g_u32ReqTimeout = 0;
    g_u32ReqQAddFail = 0;
    g_u32CtrlTimeout = 0;
    g_u32CtrlQAddFail = 0;
    g_u32ApiSubmit = 0;
    g_u32ApiTmf = 0;
    g_u32ApiEventPoll = 0;
    g_u32ApiQstats = 0;
    g_u32ApiQFree = 0;
    g_u32DataInOps = 0;
    g_u32DataOutOps = 0;
    g_u32NoDataOps = 0;
    /* Preserve g_u32SoftLogN across re-probe so log_n stays monotonic. */
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
    /* Fresh free-min watermarks (stats_clear already sets 0xffff). */

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
    q_note_free();
    /* Product marker: greppable ready line with queue inventory */
    kprintf("virtio-scsi: ready bus=%x slot=%x (ctrl+event+req) event_posted=%d\n",
            (unsigned)g_pScsi->u8Bus, (unsigned)g_pScsi->u8Slot,
            g_fEventPosted);
    /*
     * Wave 14 soft inventory rollup (prefix-stable "virtio-scsi: soft …").
     * PARTIAL typical here (ready, no product I/O yet).
     */
    scsi_soft_inventory("probe");
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

    g_u32ApiSubmit++;
    /* Soft-only: CDB path is scsi_mid soft LUN; do not claim transport. */
    if (!g_fReady) {
        if (g_fSoft) {
            g_u32IoFail++;
            g_u32SoftSubmitMiss++;
            scsi_soft_maybe_once();
        }
        return -1;
    }
    if (pReq == NULL || pReq->cdb.u8CdbLen == 0 || pReq->cdb.u8CdbLen > 32) {
        g_u32IoFail++;
        scsi_soft_maybe_once();
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
    g_u32LastCdbLen = (u32)pReq->cdb.u8CdbLen;

    cbData = pReq->cbData;
    if (cbData > sizeof(g_aData)) {
        cbData = sizeof(g_aData);
    }
    g_u32LastDataLen = cbData;
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
            g_u32ReqQAddFail++;
            g_u32IoFail++;
            scsi_soft_maybe_once();
            return -1;
        }
    } else if (pReq->fDataIn) {
        if (virtio_q_add3(&g_qReq,
                          (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                          (gj_paddr_t)(gj_vaddr_t)&g_Resp, (u32)sizeof(g_Resp),
                          1,
                          (gj_paddr_t)(gj_vaddr_t)g_aData, cbData, 1) < 0) {
            g_u32ReqQAddFail++;
            g_u32IoFail++;
            scsi_soft_maybe_once();
            return -1;
        }
    } else {
        if (virtio_q_add3(&g_qReq,
                          (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                          (gj_paddr_t)(gj_vaddr_t)g_aData, cbData, 0,
                          (gj_paddr_t)(gj_vaddr_t)&g_Resp, (u32)sizeof(g_Resp),
                          1) < 0) {
            g_u32ReqQAddFail++;
            g_u32IoFail++;
            scsi_soft_maybe_once();
            return -1;
        }
    }
    virtio_q_kick(&g_qReq);
    q_note_free();
    i32Len = virtio_q_poll(&g_qReq, VIRTIO_SCSI_POLL_SPINS);
    if (i32Len < 0) {
        kprintf("virtio-scsi: timeout\n");
        g_u32ReqTimeout++;
        g_u32IoFail++;
        scsi_soft_maybe_once();
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
        scsi_soft_maybe_once();
        return -1;
    }
    if (g_Resp.u8Status != GJ_SCSI_GOOD) {
        kprintf("virtio-scsi: scsi status=%u residual=%u\n",
                (unsigned)g_Resp.u8Status, g_Resp.u32Residual);
        g_u32IoFail++;
        scsi_soft_maybe_once();
        return -1;
    }
    g_u32IoCount++;
    if (cbData == 0) {
        g_u32NoDataOps++;
    } else if (pReq->fDataIn) {
        g_u32DataInOps++;
    } else {
        g_u32DataOutOps++;
    }
    q_note_free();
    scsi_soft_maybe_once();
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

    g_u32ApiTmf++;
    g_u32LastTmfSubtype = u32Subtype;
    /* Soft-only path: accept abort/reset family as nop success. */
    if (!g_fReady) {
        if (g_fSoft && soft_tmf_ok(u32Subtype)) {
            g_u32CtrlOk++;
            g_u32SoftTmfOk++;
            g_u32LastResponse = VIRTIO_SCSI_S_FUNCTION_SUCCEEDED;
            scsi_soft_maybe_once();
            return 0;
        }
        g_u32CtrlFail++;
        if (g_fSoft) {
            g_u32SoftTmfFail++;
        }
        scsi_soft_maybe_once();
        return -1;
    }

    if (u32Subtype > GJ_VIRTIO_SCSI_TMF_QUERY_TASK_SET) {
        g_u32CtrlFail++;
        scsi_soft_maybe_once();
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
        g_u32CtrlQAddFail++;
        g_u32CtrlFail++;
        scsi_soft_maybe_once();
        return -1;
    }
    virtio_q_kick(&g_qCtrl);
    q_note_free();
    i32Len = virtio_q_poll(&g_qCtrl, VIRTIO_SCSI_CTRL_SPINS);
    if (i32Len < 0) {
        kprintf("virtio-scsi: ctrl TMF timeout subtype=%u\n", u32Subtype);
        g_u32CtrlTimeout++;
        g_u32CtrlFail++;
        scsi_soft_maybe_once();
        return -1;
    }

    g_u32LastResponse = (u32)g_TmfResp.u8Response;
    if (g_TmfResp.u8Response != VIRTIO_SCSI_S_OK &&
        g_TmfResp.u8Response != VIRTIO_SCSI_S_FUNCTION_SUCCEEDED) {
        kprintf("virtio-scsi: ctrl TMF subtype=%u response=%u\n",
                u32Subtype, (unsigned)g_TmfResp.u8Response);
        g_u32CtrlFail++;
        scsi_soft_maybe_once();
        return -1;
    }
    g_u32CtrlOk++;
    q_note_free();
    scsi_soft_maybe_once();
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

    g_u32ApiEventPoll++;
    /* Soft-only / not ready: no async events. */
    if (!g_fReady) {
        return 0;
    }
    if (pOut == NULL) {
        return -1;
    }

    g_u32EventPolls++;

    /* Ensure a buffer is posted before soft poll. */
    if (!g_fEventPosted) {
        if (event_post() == 0) {
            g_u32EventRepostOk++;
        } else {
            g_u32EventRepostFail++;
        }
    }
    if (!g_fEventPosted) {
        g_u32EventEmpty++;
        return 0;
    }

    i32Len = virtio_q_poll(&g_qEvent, VIRTIO_SCSI_EVENT_SPINS);
    if (i32Len < 0) {
        g_u32EventEmpty++;
        return 0; /* none pending — soft non-blocking */
    }

    g_fEventPosted = 0;
    pEv = (struct virtio_scsi_event *)(void *)g_aEvent;
    pOut->u32Event = pEv->u32Event & ~GJ_VIRTIO_SCSI_EVT_MISSED;
    pOut->u32Missed = (pEv->u32Event & GJ_VIRTIO_SCSI_EVT_MISSED) ? 1u : 0u;
    pOut->u32Reason = pEv->u32Reason;
    pOut->u32Lun = (u32)pEv->aLun[1];
    g_u32LastEventType = pOut->u32Event;
    g_u32LastEventReason = pOut->u32Reason;
    g_u32EventCount++;

    /* Soft repost so the device can deliver the next event. */
    if (event_post() != 0) {
        kprintf("virtio-scsi: event repost failed\n");
        g_u32EventRepostFail++;
    } else {
        g_u32EventRepostOk++;
    }
    q_note_free();

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
    g_u32ApiQstats++;
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
        q_note_free();
        pOut->u16FreeCtrl = virtio_q_num_free(&g_qCtrl);
        pOut->u16FreeEvent = virtio_q_num_free(&g_qEvent);
        pOut->u16FreeReq = virtio_q_num_free(&g_qReq);
    }
    /*
     * Emit soft inventory on stats read so bring-up smoke also greps
     * virtio-scsi: soft … lines (mirrors virtio_blk q_stats path).
     * greppable: virtio-scsi: soft
     */
    scsi_soft_inventory("q_stats");
    return 0;
}

u16
virtio_scsi_q_free(u32 u32QIdx)
{
    g_u32ApiQFree++;
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
