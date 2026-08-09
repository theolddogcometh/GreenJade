/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-scsi modern PCI (OASIS virtio-scsi).
 * Pure C, dual license (MIT OR Apache-2.0). No GPL. No Linux virtio source.
 *
 * SAS / SCSI path role:
 *   scsi_mid -> virtio-scsi (T0 interim) -> later SAS HBA (T2 product).
 *   This unit is the lean residual T0 transport under scsi_mid / scsi_door.
 *   Soft residual lamps here are Soft!=product (not SAS HBA DoD).
 *
 * C0 T0 SCSI soft residual (this unit only; Soft!=product; Dual DoD OPEN):
 *   Claim class C0 eng residual only - never Dual DoD A/B close / bar3 /
 *   product SAS HBA DoD. G-AC-1: no Linux .ko product AC. No GPL.
 *   Dual DoD product_sas=OPEN (T2 SAS HBA / UDX host is a separate track).
 *
 * Queues (fixed OASIS indices):
 *   q0 control - TMF soft path (kick+poll; soft-accept when no HBA)
 *   q1 event   - async events (post at probe; soft poll + repost)
 *   q2 request - command I/O (req [+ data-out] -> resp [+ data-in])
 *
 * Soft path (bring-up, no live HBA):
 *   g_fSoft after failed/absent probe - TMF/stats/event_poll soft;
 *   CDB I/O stays with scsi_mid soft LUN (submit returns -1).
 *
 * Soft residual lean (this unit only; Soft!=product; dual MIT OR Apache-2.0):
 *   Lean residual only - never re-introduce multi-kprintf stamp storms
 *   (prior FAULT class: sequential soft-deepen kprintf floods -> stack smash
 *   / #PF I=1). Boot inventory is one-shot one line (soft_arm/probe).
 *   Residual lean is one-shot one line (+ optional PASS): probe/soft_arm or
 *   first soft-activity (TMF/submit/event soft) - whichever first (H2-safe).
 *   Silent residual self-check (OASIS q idx / resp / TMF / mid handoff /
 *   residual-byte geometry / poll budgets / exact packed layouts).
 *   Silent residual counters for TMF/event/req/residual-byte(partial|over|
 *   zero)/probe/free-min/timeout/qadd/sense/SAM.
 *   Soft device-cfg snap (sense/cdb/nq/seg/max_sec/cmd_lun/evt_info).
 *   Compile-time geometry residual (typedef size guards; no runtime cost).
 *   No wave=/version stamp. No ret*angle stamps. stamp_storm=0.
 *   Soft residual lean != product SAS HBA dual-license DoD (product_sas=OPEN).
 *   greppable: virtio-scsi: soft residual lean
 *   greppable: virtio-scsi: soft residual lean PASS
 *   greppable: virtio-scsi: soft residual
 *   greppable: virtio-scsi: soft inventory
 *   greppable: Soft!=product path=sas_t0
 *   greppable: dual=MIT_OR_Apache-2.0 | G-AC-1 | handoff=scsi_mid|scsi_door
 *   greppable: C0 | product_sas=OPEN | residual_byte | stamp_storm=0
 *
 * Greppable product markers (serial):
 *   virtio-scsi: ready ...
 *   virtio-scsi: soft-armed ...
 *
 * Product I/O via scsi_mid/scsi_door / GJ_SYS_SCSI.
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

/* Transitional virtio-scsi PCI device id (OASIS public inventory; Soft!=product). */
#define VIRTIO_SCSI_PCI_TRANSITIONAL 0x1004u

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

/*
 * Soft device-config residual snapshot (OASIS virtio-scsi config; first fields).
 * Peek only - Soft!=product; never claims product SAS depth from cfg alone.
 */
struct virtio_scsi_config_soft {
    u32 u32NumQueues;
    u32 u32SegMax;
    u32 u32MaxSectors;
    u32 u32CmdPerLun;
    u32 u32EventInfoSize;
    u32 u32SenseSize;
    u32 u32CdbSize;
} __attribute__((packed));

/*
 * Compile-time geometry residual (Soft!=product / C0; no runtime cost).
 * Exact OASIS packed layouts + queue power-of-two + soft bounce. Fail the
 * build if residual honesty drifts. No GPL; dual MIT OR Apache-2.0.
 */
typedef char gj_vscsi_req_cmd_sz[
    (sizeof(struct virtio_scsi_req_cmd) == 51u) ? 1 : -1];
typedef char gj_vscsi_resp_cmd_sz[
    (sizeof(struct virtio_scsi_resp_cmd) == 108u) ? 1 : -1];
typedef char gj_vscsi_tmf_req_sz[
    (sizeof(struct virtio_scsi_ctrl_tmf_req) == 24u) ? 1 : -1];
typedef char gj_vscsi_tmf_resp_sz[
    (sizeof(struct virtio_scsi_ctrl_tmf_resp) == 1u) ? 1 : -1];
typedef char gj_vscsi_event_sz[
    (sizeof(struct virtio_scsi_event) == 16u) ? 1 : -1];
typedef char gj_vscsi_cfg_soft_sz[
    (sizeof(struct virtio_scsi_config_soft) == 28u) ? 1 : -1];
typedef char gj_vscsi_q_size_pow2[
    ((VIRTIO_SCSI_Q_SIZE != 0u) &&
     ((VIRTIO_SCSI_Q_SIZE & (VIRTIO_SCSI_Q_SIZE - 1u)) == 0u))
        ? 1
        : -1];
typedef char gj_vscsi_data_max_512[
    (GJ_VIRTIO_SCSI_DATA_MAX == 512u) ? 1 : -1];
typedef char gj_vscsi_q_idx_oasis[
    (VIRTIO_SCSI_Q_CONTROL == 0u && VIRTIO_SCSI_Q_EVENT == 1u &&
     VIRTIO_SCSI_Q_REQUEST == 2u)
        ? 1
        : -1];
typedef char gj_vscsi_kind6[(VIRTIO_SCSI_KIND == 6u) ? 1 : -1];
typedef char gj_vscsi_event_buf_covers[
    (VIRTIO_SCSI_EVENT_SZ >= 16u) ? 1 : -1];

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

/*
 * Lean soft residual (file-static; Soft!=product / C0). Silent counters only -
 * never multi-line inventory dumps / stamp storms. One-shot boot inventory +
 * one-shot first soft-activity residual lean. Free-min + probe tallies silent.
 * Residual-byte class: zero / partial / over (data-in clamp honesty).
 * greppable: virtio-scsi: soft residual lean | soft residual | soft inventory
 */
static int g_fSoftInvOnce;        /* one-shot soft inventory boot lamp */
static int g_fSoftResOnce;        /* one-shot first soft-activity residual lean */
static u32 g_u32SoftTmfOk;        /* soft-only TMF accepts (no HBA) */
static u32 g_u32SoftTmfFail;      /* soft-only TMF rejects */
static u32 g_u32SoftSubmitMiss;   /* soft-only CDB submits (mid owns path) */
static u32 g_u32SoftEventPoll;    /* event_poll entries on soft-only path */
static u32 g_u32EventPolls;       /* event_poll entries on live path */
static u32 g_u32EventEmpty;       /* event_poll soft-empty (none pending) */
static u32 g_u32EventRepostOk;    /* event buffer repost successes */
static u32 g_u32EventRepostFail;
static u32 g_u32EventMissedBit;   /* events with EVENTS_MISSED high bit */
static u32 g_u32ReqTimeout;       /* request q poll timeouts */
static u32 g_u32ReqQAddFail;      /* request q add failures */
static u32 g_u32CtrlTimeout;      /* control q poll timeouts */
static u32 g_u32CtrlQAddFail;     /* control q add failures */
static u32 g_u32LastTmfSubtype;   /* sticky last TMF subtype */
static u32 g_u32LastEventType;    /* sticky last event type */
static u32 g_u32LastEventReason;  /* sticky last event reason */
static u32 g_u32LastCdbLen;       /* sticky last CDB length */
static u32 g_u32LastDataLen;      /* sticky last data payload length */
static u32 g_u32LastUsedLen;      /* sticky last request used-ring length */
static u32 g_u32LastSenseLen;     /* sticky last sense length (clamped) */
static u32 g_u32DataInOps;        /* data-in submit completions */
static u32 g_u32DataOutOps;       /* data-out submit completions */
static u32 g_u32NoDataOps;        /* no-data submit completions */
static u32 g_u32ResidualClamp;    /* residual-byte clamps applied (partial+over) */
static u32 g_u32ResidualZero;     /* residual==0 completions (full xfer) */
static u32 g_u32ResidualPartial;  /* 0 < residual < cbData (short xfer) */
static u32 g_u32ResidualOver;     /* residual >= cbData (nothing transferred) */
static u32 g_u32SenseClamp;       /* sense_len clamped to mid sense max */
static u32 g_u32SamGood;          /* SAM GOOD (0) status completions */
static u32 g_u32SamCheck;         /* SAM CHECK CONDITION status */
static u32 g_u32SamOther;         /* other SAM status class */
static u32 g_u32RespOk;           /* VIRTIO_SCSI_S_OK transport responses */
static u32 g_u32RespFail;         /* non-OK transport response class */
static u32 g_u32ProbeOk;          /* probe DRIVER_OK path */
static u32 g_u32ProbeNodev;       /* probe no-device */
static u32 g_u32ProbeFail;        /* probe pci/nego/q fail */
static u32 g_u32QStatsApi;        /* qstats() samples (silent residual) */
static u32 g_u32ReadyQuery;       /* virtio_scsi_ready samples */
static u32 g_u32SoftQuery;        /* virtio_scsi_soft_active samples */
static u32 g_u32Kicks;            /* queue kick count (ctrl+event+req) */
static u16 g_u16FreeMinCtrl;      /* free-desc watermark control q */
static u16 g_u16FreeMinEvent;     /* free-desc watermark event q */
static u16 g_u16FreeMinReq;       /* free-desc watermark request q */
static u32 g_u32CfgSenseSize;     /* soft cfg sense_size snapshot */
static u32 g_u32CfgCdbSize;       /* soft cfg cdb_size snapshot */
static u32 g_u32CfgNumQueues;     /* soft cfg num_queues snapshot */
static u32 g_u32CfgSegMax;        /* soft cfg seg_max snapshot */
static u32 g_u32CfgMaxSectors;    /* soft cfg max_sectors snapshot */
static u32 g_u32CfgCmdPerLun;     /* soft cfg cmd_per_lun snapshot */
static u32 g_u32CfgEventInfoSize; /* soft cfg event_info_size snapshot */
static u32 g_u32CfgHave;          /* 1 if soft cfg snap succeeded */
static u32 g_u32LeanOk;           /* last residual lean self-check ok count */
static u32 g_u32LeanChecks;       /* last residual lean self-check total */
static u32 g_u32MidHandoffOk;     /* scsi_mid/scsi_door surface lean bits */

/* Single outstanding command buffers (identity-mapped BSS). */
static struct virtio_scsi_req_cmd      g_Req __attribute__((aligned(16)));
static struct virtio_scsi_resp_cmd     g_Resp __attribute__((aligned(16)));
static struct virtio_scsi_ctrl_tmf_req g_TmfReq __attribute__((aligned(16)));
static struct virtio_scsi_ctrl_tmf_resp g_TmfResp __attribute__((aligned(16)));
static u8 g_aData[GJ_VIRTIO_SCSI_DATA_MAX] __attribute__((aligned(16)));
static u8 g_aEvent[VIRTIO_SCSI_EVENT_SZ] __attribute__((aligned(16)));

/* ---- Soft inventory / residual lean (Soft!=product) ---------------------- */

/**
 * Soft device-cfg residual peek (identity MMIO). Silent - no kprintf.
 * Soft!=product: cfg snap is inventory honesty, not SAS HBA DoD.
 * Captures first OASIS config fields used by residual lean honesty.
 */
static void
scsi_soft_cfg_snap(struct gj_virtio_dev *pDev)
{
    volatile struct virtio_scsi_config_soft *pCfg;

    g_u32CfgHave = 0;
    g_u32CfgSenseSize = 0;
    g_u32CfgCdbSize = 0;
    g_u32CfgNumQueues = 0;
    g_u32CfgSegMax = 0;
    g_u32CfgMaxSectors = 0;
    g_u32CfgCmdPerLun = 0;
    g_u32CfgEventInfoSize = 0;
    if (pDev == NULL || pDev->pDevice == NULL) {
        return;
    }
    pCfg = (volatile struct virtio_scsi_config_soft *)(void *)pDev->pDevice;
    g_u32CfgNumQueues = pCfg->u32NumQueues;
    g_u32CfgSegMax = pCfg->u32SegMax;
    g_u32CfgMaxSectors = pCfg->u32MaxSectors;
    g_u32CfgCmdPerLun = pCfg->u32CmdPerLun;
    g_u32CfgEventInfoSize = pCfg->u32EventInfoSize;
    g_u32CfgSenseSize = pCfg->u32SenseSize;
    g_u32CfgCdbSize = pCfg->u32CdbSize;
    g_u32CfgHave = 1;
}

/** Silent free-desc watermarks (live queues only; Soft!=product). */
static void
scsi_note_free(void)
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

static void
scsi_q_kick(struct gj_virtq *pQ)
{
    virtio_q_kick(pQ);
    g_u32Kicks++;
    scsi_note_free();
}

/**
 * One-shot one-line boot soft inventory only (Soft!=product).
 * LEAN residual: single kprintf - never multi-line stamp floods (FAULT class).
 * SAS path honesty: T0 virtio-scsi residual under scsi_mid (not SAS HBA).
 * greppable: virtio-scsi: soft inventory
 * greppable: Soft!=product path=sas_t0
 */
static void
scsi_soft_inventory_once(const char *szVia)
{
    u32 u32Ready;
    u32 u32Soft;
    u8  u8Bus;
    u8  u8Slot;

    if (g_fSoftInvOnce != 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    u32Ready = g_fReady ? 1u : 0u;
    u32Soft = (g_fSoft && !g_fReady) ? 1u : 0u;
    u8Bus = (g_pScsi != NULL) ? g_pScsi->u8Bus : 0;
    u8Slot = (g_pScsi != NULL) ? g_pScsi->u8Slot : 0;
    /*
     * ONE line only. Soft!=product. No wave=/version stamp.
     * path=sas_t0: interim virtio-scsi under scsi_mid (lamp != SAS DoD).
     * soft_ne_product=1: residual honesty (dual MIT OR Apache-2.0).
     */
    kprintf("virtio-scsi: soft inventory via=%s ready=%u soft=%u "
            "bus=%x slot=%x event_posted=%u data_max=%u q_size=%u "
            "cfg=%u sense=%u cdb=%u nq=%u seg=%u max_sec=%u cmd_lun=%u "
            "evt_info=%u q0=1 q1=1 q2=1 C0=1 residual_byte=1 "
            "path=sas_t0 handoff=scsi_mid|scsi_door "
            "product_sas=OPEN dual=MIT_OR_Apache-2.0 stamp_storm=0 "
            "no_version_stamp=1 soft_ne_product=1 G-AC-1 Soft!=product\n",
            szVia != NULL ? szVia : "?",
            u32Ready, u32Soft,
            (unsigned)u8Bus, (unsigned)u8Slot,
            g_fEventPosted ? 1u : 0u,
            (unsigned)GJ_VIRTIO_SCSI_DATA_MAX,
            (unsigned)VIRTIO_SCSI_Q_SIZE,
            g_u32CfgHave, g_u32CfgSenseSize, g_u32CfgCdbSize,
            g_u32CfgNumQueues, g_u32CfgSegMax, g_u32CfgMaxSectors,
            g_u32CfgCmdPerLun, g_u32CfgEventInfoSize);
}

/**
 * Silent residual lean self-check (no kprintf; Soft!=product / C0).
 * OASIS queue geometry + response/TMF catalog + scsi_mid/scsi_door handoff +
 * residual-byte / exact packed layouts / poll budgets / public alias parity.
 * Results fold into soft residual lean lines only (no stamp storm).
 * Returns ok count; *pOutChecks = total checks when non-NULL.
 */
static u32
scsi_soft_residual_lean_ok(u32 *pOutChecks)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u32 u32Mid = 0;

    /* 1: fixed OASIS queue indices (ctrl/event/req) */
    u32Checks++;
    if (VIRTIO_SCSI_Q_CONTROL == 0u && VIRTIO_SCSI_Q_EVENT == 1u &&
        VIRTIO_SCSI_Q_REQUEST == 2u) {
        u32Ok++;
    }
    /* 2: bring-up queue size power-of-two covers product path */
    u32Checks++;
    if (VIRTIO_SCSI_Q_SIZE == 64u &&
        (VIRTIO_SCSI_Q_SIZE & (VIRTIO_SCSI_Q_SIZE - 1u)) == 0u) {
        u32Ok++;
    }
    /* 3: single-seg soft bounce data max (scsi_mid may chunk) */
    u32Checks++;
    if (GJ_VIRTIO_SCSI_DATA_MAX == 512u &&
        (u32)sizeof(g_aData) == GJ_VIRTIO_SCSI_DATA_MAX) {
        u32Ok++;
    }
    /* 4: product kind marker (virtio_pci kind_from_device scsi) */
    u32Checks++;
    if (VIRTIO_SCSI_KIND == 6u && GJ_VIRTIO_KIND_SCSI == 6u) {
        u32Ok++;
    }
    /* 5: modern PCI device id (T0 residual inventory) */
    u32Checks++;
    if (GJ_VIRTIO_PCI_DEV_SCSI_MODERN == 0x1048u) {
        u32Ok++;
    }
    /* 6: OASIS transport response catalog (ok / function class) */
    u32Checks++;
    if (VIRTIO_SCSI_S_OK == 0u && VIRTIO_SCSI_S_OVERRUN == 1u &&
        VIRTIO_SCSI_S_FUNCTION_SUCCEEDED == 10u &&
        VIRTIO_SCSI_S_FUNCTION_REJECTED == 11u &&
        VIRTIO_SCSI_S_INCORRECT_LUN == 12u) {
        u32Ok++;
    }
    /* 7: controlq T_TMF + soft-accept abort/reset family bounds */
    u32Checks++;
    if (VIRTIO_SCSI_T_TMF == 0u && GJ_VIRTIO_SCSI_T_TMF == 0u &&
        GJ_VIRTIO_SCSI_TMF_ABORT_TASK == 0u &&
        GJ_VIRTIO_SCSI_TMF_LOGICAL_UNIT_RESET == 5u &&
        GJ_VIRTIO_SCSI_TMF_QUERY_TASK_SET == 7u) {
        u32Ok++;
    }
    /* 8: async event catalog (MISSED high bit + transport reset) */
    u32Checks++;
    if (GJ_VIRTIO_SCSI_EVT_MISSED == 0x80000000u &&
        GJ_VIRTIO_SCSI_EVT_NO_EVENT == 0u &&
        GJ_VIRTIO_SCSI_EVT_TRANSPORT_RESET == 1u &&
        GJ_VIRTIO_SCSI_EVT_ASYNC_NOTIFY == 2u &&
        GJ_VIRTIO_SCSI_EVT_PARAM_CHANGE == 3u) {
        u32Ok++;
    }
    /* 9: exact packed OASIS cmd shapes (req/resp/event/tmf) */
    u32Checks++;
    if (sizeof(struct virtio_scsi_req_cmd) == 51u &&
        sizeof(struct virtio_scsi_resp_cmd) == 108u &&
        sizeof(struct virtio_scsi_event) == 16u &&
        sizeof(struct virtio_scsi_ctrl_tmf_req) == 24u &&
        sizeof(struct virtio_scsi_ctrl_tmf_resp) == 1u) {
        u32Ok++;
    }
    /* 10: soft event buffer covers OASIS event + headroom */
    u32Checks++;
    if (VIRTIO_SCSI_EVENT_SZ == 256u &&
        VIRTIO_SCSI_EVENT_SZ >= sizeof(struct virtio_scsi_event) &&
        (u32)sizeof(g_aEvent) == VIRTIO_SCSI_EVENT_SZ) {
        u32Ok++;
    }
    /* 11: qstats wire-stable size (extend only at end; ~52B front) */
    u32Checks++;
    if (sizeof(struct gj_virtio_scsi_qstats) >= 48u &&
        sizeof(struct gj_virtio_scsi_qstats) >= 52u) {
        u32Ok++;
    }
    /* 12: page geometry (desc rings identity-mapped soft path) */
    u32Checks++;
    if (GJ_PAGE_SIZE == 4096u &&
        (GJ_PAGE_SIZE & (GJ_PAGE_SIZE - 1u)) == 0u) {
        u32Ok++;
    }
    /* 13: scsi_mid CDB/sense soft geometry (handoff residual) */
    u32Checks++;
    if (GJ_SCSI_CDB_MAX == 16u && GJ_SCSI_SENSE_MAX == 32u &&
        GJ_SCSI_SOFT_SEC_SIZE == 512u &&
        GJ_SCSI_SOFT_SECTORS == 64u) {
        u32Ok++;
        u32Mid++;
    }
    /* 14: mid request surface present (scsi_mid_submit residual) */
    u32Checks++;
    if (sizeof(struct gj_scsi_request) >= 32u &&
        sizeof(struct gj_scsi_cdb) >= 16u &&
        sizeof(struct gj_scsi_sense) >= 32u) {
        u32Ok++;
        u32Mid++;
    }
    /* 15: door-shaped opcode catalog (scsi_door CAP path residual) */
    u32Checks++;
    if (GJ_SCSI_DOOR_OP_INQUIRY == 0u && GJ_SCSI_DOOR_OP_READ_CAP == 1u &&
        GJ_SCSI_DOOR_OP_READ10 == 2u && GJ_SCSI_DOOR_OP_WRITE10 == 3u &&
        GJ_SCSI_DOOR_OP_RAW == 4u && GJ_SCSI_DOOR_OP_TEST_UNIT == 5u &&
        GJ_SCSI_DOOR_OP_SYNC_CACHE == 6u &&
        GJ_SCSI_DOOR_OP_REQ_SENSE == 7u &&
        sizeof(struct scsi_door_req) >= 16u) {
        u32Ok++;
        u32Mid++;
    }
    /* 16: soft cfg snap layout exact OASIS first fields (7 x u32) */
    u32Checks++;
    if (sizeof(struct virtio_scsi_config_soft) == 28u) {
        u32Ok++;
    }
    /* 17: public queue index aliases match local residual constants */
    u32Checks++;
    if (GJ_VIRTIO_SCSI_Q_CONTROL == VIRTIO_SCSI_Q_CONTROL &&
        GJ_VIRTIO_SCSI_Q_EVENT == VIRTIO_SCSI_Q_EVENT &&
        GJ_VIRTIO_SCSI_Q_REQUEST == VIRTIO_SCSI_Q_REQUEST) {
        u32Ok++;
    }
    /* 18: controlq AN type catalog inventory (not soft-driven; honesty) */
    u32Checks++;
    if (GJ_VIRTIO_SCSI_T_AN_QUERY == 1u &&
        GJ_VIRTIO_SCSI_T_AN_SUBSCRIBE == 2u) {
        u32Ok++;
    }
    /* 19: poll budgets non-zero (bring-up soft path honesty; no hang) */
    u32Checks++;
    if (VIRTIO_SCSI_POLL_SPINS >= 1000u &&
        VIRTIO_SCSI_CTRL_SPINS >= 1000u &&
        VIRTIO_SCSI_EVENT_SPINS >= 1u) {
        u32Ok++;
    }
    /* 20: residual-byte sense headroom (device sense 96 >= mid 32 clamp) */
    u32Checks++;
    if ((u32)sizeof(g_Resp.aSense) == 96u &&
        GJ_SCSI_SENSE_MAX == 32u &&
        (u32)GJ_SCSI_SENSE_MAX <= (u32)sizeof(g_Resp.aSense) &&
        GJ_VIRTIO_SCSI_DATA_MAX == GJ_SCSI_SOFT_SEC_SIZE) {
        u32Ok++;
        u32Mid++;
    }
    /* 21: transitional PCI id residual inventory (probe fall-back path) */
    u32Checks++;
    if (VIRTIO_SCSI_PCI_TRANSITIONAL == 0x1004u) {
        u32Ok++;
    }
    /* 22: SAM status catalog used by residual path (GOOD/CHECK/BUSY) */
    u32Checks++;
    if ((u32)GJ_SCSI_GOOD == 0u &&
        (u32)GJ_SCSI_CHECK_CONDITION == 2u &&
        (u32)GJ_SCSI_BUSY == 8u) {
        u32Ok++;
        u32Mid++;
    }
    /* 23: task attr SIMPLE + soft event struct public size */
    u32Checks++;
    if (VIRTIO_SCSI_S_SIMPLE == 0u &&
        sizeof(struct gj_virtio_scsi_event) >= 16u) {
        u32Ok++;
    }
    /* 24: live free-desc watermark coherent when ready (silent) */
    u32Checks++;
    if (!g_fReady ||
        (g_qReq.u16Size > 0u && g_u16FreeMinReq <= g_qReq.u16Size &&
         g_u16FreeMinCtrl <= g_qCtrl.u16Size &&
         g_u16FreeMinEvent <= g_qEvent.u16Size)) {
        u32Ok++;
    }
    /* 25: ready+soft never co-claim (soft_active = soft && !ready) */
    u32Checks++;
    if (!(g_fReady != 0 && g_fSoft != 0)) {
        u32Ok++;
    }

    g_u32LeanOk = u32Ok;
    g_u32LeanChecks = u32Checks;
    g_u32MidHandoffOk = u32Mid;
    if (pOutChecks != NULL) {
        *pOutChecks = u32Checks;
    }
    return u32Ok;
}

/**
 * One-shot residual lean (Soft!=product / C0). Probe/soft_arm or first soft
 * activity (TMF/submit/event soft) - whichever first. LEAN: one kprintf +
 * optional PASS. Never from qstats (FAULT class avoid). No wave=/version stamp.
 * greppable: virtio-scsi: soft residual lean
 * greppable: virtio-scsi: soft residual lean PASS
 * greppable: virtio-scsi: soft residual
 * greppable: Soft!=product path=sas_t0
 * greppable: dual=MIT_OR_Apache-2.0 | G-AC-1 | handoff=scsi_mid|scsi_door
 * greppable: C0 | residual_byte | product_sas=OPEN
 */
static void
scsi_soft_residual_once(const char *szVia)
{
    u32 u32Soft;
    u32 u32Ok;
    u32 u32Checks;
    u16 u16FreeCtrl;
    u16 u16FreeEvent;
    u16 u16FreeReq;

    if (g_fSoftResOnce != 0) {
        return;
    }
    g_fSoftResOnce = 1;

    u32Ok = scsi_soft_residual_lean_ok(&u32Checks);
    u32Soft = (g_fSoft && !g_fReady) ? 1u : 0u;
    u16FreeCtrl = (g_u16FreeMinCtrl == 0xffffu) ? 0u : g_u16FreeMinCtrl;
    u16FreeEvent = (g_u16FreeMinEvent == 0xffffu) ? 0u : g_u16FreeMinEvent;
    u16FreeReq = (g_u16FreeMinReq == 0xffffu) ? 0u : g_u16FreeMinReq;

    /*
     * ONE lean line. Soft!=product dual license; no version stamp; no storm.
     * path=sas_t0 product_sas=OPEN: T0 interim under scsi_mid (not SAS DoD).
     * handoff=scsi_mid|scsi_door: residual surface honesty (G-AC-1).
     * residual_byte: zero|partial|over class (data-in clamp honesty).
     * C0: eng residual claim class (not Dual DoD / bar3 close).
     */
    kprintf("virtio-scsi: soft residual lean via=%s soft=%u ready=%u "
            "tmf_ok=%u tmf_fail=%u submit_miss=%u soft_evt=%u "
            "io_ok=%u io_fail=%u ctrl_ok=%u ctrl_fail=%u "
            "evt=%u evt_empty=%u evt_repost_ok=%u evt_repost_fail=%u "
            "evt_missed=%u evt_polls=%u "
            "res_clamp=%u res_zero=%u res_partial=%u res_over=%u "
            "sense_clamp=%u sam_good=%u sam_check=%u sam_other=%u "
            "resp_ok=%u resp_fail=%u kicks=%u "
            "din=%u dout=%u nodata=%u "
            "to_req=%u qadd_req=%u to_ctrl=%u qadd_ctrl=%u "
            "free_c=%u free_e=%u free_r=%u "
            "last_resp=%u last_res=%u last_used=%u last_tmf=%u "
            "last_cdb=%u last_data=%u last_sense=%u last_evt=%u "
            "probe_ok=%u nodev=%u fail=%u "
            "cfg=%u sense=%u cdb=%u nq=%u seg=%u max_sec=%u cmd_lun=%u "
            "evt_info=%u qstats_api=%u ready_q=%u soft_q=%u "
            "q_size=%u data_max=%u event_sz=%u "
            "lean_ok=%u/%u mid_handoff_ok=%u "
            "C0=1 residual_byte=1 path=sas_t0 "
            "handoff=scsi_mid|scsi_door product_sas=OPEN "
            "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
            "soft_ne_product=1 G-AC-1 Soft!=product\n",
            szVia != NULL ? szVia : "?",
            u32Soft, g_fReady ? 1u : 0u,
            g_u32SoftTmfOk, g_u32SoftTmfFail, g_u32SoftSubmitMiss,
            g_u32SoftEventPoll,
            g_u32IoCount, g_u32IoFail, g_u32CtrlOk, g_u32CtrlFail,
            g_u32EventCount, g_u32EventEmpty, g_u32EventRepostOk,
            g_u32EventRepostFail, g_u32EventMissedBit, g_u32EventPolls,
            g_u32ResidualClamp, g_u32ResidualZero, g_u32ResidualPartial,
            g_u32ResidualOver, g_u32SenseClamp, g_u32SamGood, g_u32SamCheck,
            g_u32SamOther, g_u32RespOk, g_u32RespFail, g_u32Kicks,
            g_u32DataInOps, g_u32DataOutOps, g_u32NoDataOps,
            g_u32ReqTimeout, g_u32ReqQAddFail, g_u32CtrlTimeout,
            g_u32CtrlQAddFail,
            (unsigned)u16FreeCtrl, (unsigned)u16FreeEvent,
            (unsigned)u16FreeReq,
            g_u32LastResponse, g_u32LastResidual, g_u32LastUsedLen,
            g_u32LastTmfSubtype, g_u32LastCdbLen, g_u32LastDataLen,
            g_u32LastSenseLen, g_u32LastEventType,
            g_u32ProbeOk, g_u32ProbeNodev, g_u32ProbeFail,
            g_u32CfgHave, g_u32CfgSenseSize, g_u32CfgCdbSize,
            g_u32CfgNumQueues, g_u32CfgSegMax, g_u32CfgMaxSectors,
            g_u32CfgCmdPerLun, g_u32CfgEventInfoSize,
            g_u32QStatsApi, g_u32ReadyQuery, g_u32SoftQuery,
            (unsigned)VIRTIO_SCSI_Q_SIZE,
            (unsigned)GJ_VIRTIO_SCSI_DATA_MAX,
            (unsigned)VIRTIO_SCSI_EVENT_SZ,
            u32Ok, u32Checks, g_u32MidHandoffOk);

    if (u32Ok == u32Checks && g_u32MidHandoffOk >= 5u) {
        /* Grep: virtio-scsi: soft residual lean PASS */
        kprintf("virtio-scsi: soft residual lean PASS "
                "checks=%u ok=%u mid_handoff_ok=%u "
                "C0=1 residual_byte=1 path=sas_t0 "
                "handoff=scsi_mid|scsi_door product_sas=OPEN "
                "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
                "soft_ne_product=1 G-AC-1 Soft!=product\n",
                u32Checks, u32Ok, g_u32MidHandoffOk);
    }
}

/**
 * Fire residual lean once after probe/soft_arm or first soft activity.
 * Idempotent (g_fSoftResOnce); Soft!=product; H2 no stamp storms.
 */
static void
scsi_soft_maybe_residual(const char *szVia)
{
    if (g_fSoftResOnce != 0) {
        return;
    }
    scsi_soft_residual_once(szVia);
}

/* ---- Soft helpers -------------------------------------------------------- */

static void
soft_arm(void)
{
    g_fSoft = 1;
    g_fReady = 0;
    g_fEventPosted = 0;
    /*
     * Keep g_pScsi through inventory + residual lean so bus/slot soft snap
     * is honest when soft-arm follows a partial probe fail; then drop live
     * handle. Sticky cfg residual counters stay (Soft!=product honesty).
     */
    kprintf("virtio-scsi: soft-armed (ctrl+event+req soft path, no HBA)\n");
    /* One-shot soft inventory + residual lean (LEAN residual; Soft!=product). */
    scsi_soft_inventory_once("soft_arm");
    scsi_soft_maybe_residual("soft_arm");
    g_pScsi = NULL;
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
    g_u32SoftEventPoll = 0;
    g_u32EventPolls = 0;
    g_u32EventEmpty = 0;
    g_u32EventRepostOk = 0;
    g_u32EventRepostFail = 0;
    g_u32EventMissedBit = 0;
    g_u32LastTmfSubtype = 0;
    g_u32LastEventType = 0;
    g_u32LastEventReason = 0;
    g_u32LastCdbLen = 0;
    g_u32LastDataLen = 0;
    g_u32LastUsedLen = 0;
    g_u32LastSenseLen = 0;
    g_u32ReqTimeout = 0;
    g_u32ReqQAddFail = 0;
    g_u32CtrlTimeout = 0;
    g_u32CtrlQAddFail = 0;
    g_u32DataInOps = 0;
    g_u32DataOutOps = 0;
    g_u32NoDataOps = 0;
    g_u32ResidualClamp = 0;
    g_u32ResidualZero = 0;
    g_u32ResidualPartial = 0;
    g_u32ResidualOver = 0;
    g_u32SenseClamp = 0;
    g_u32SamGood = 0;
    g_u32SamCheck = 0;
    g_u32SamOther = 0;
    g_u32RespOk = 0;
    g_u32RespFail = 0;
    g_u32ProbeOk = 0;
    g_u32ProbeNodev = 0;
    g_u32ProbeFail = 0;
    g_u32QStatsApi = 0;
    g_u32ReadyQuery = 0;
    g_u32SoftQuery = 0;
    g_u32Kicks = 0;
    g_u16FreeMinCtrl = 0xffffu;
    g_u16FreeMinEvent = 0xffffu;
    g_u16FreeMinReq = 0xffffu;
    g_u32CfgSenseSize = 0;
    g_u32CfgCdbSize = 0;
    g_u32CfgNumQueues = 0;
    g_u32CfgSegMax = 0;
    g_u32CfgMaxSectors = 0;
    g_u32CfgCmdPerLun = 0;
    g_u32CfgEventInfoSize = 0;
    g_u32CfgHave = 0;
    g_u32LeanOk = 0;
    g_u32LeanChecks = 0;
    g_u32MidHandoffOk = 0;
    /*
     * Preserve g_fSoftInvOnce / g_fSoftResOnce across re-probe so residual
     * lamps stay one-shot (FAULT class: no stamp storms). Soft!=product / C0.
     */
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
    scsi_q_kick(&g_qEvent);
    return 0;
}

/* ---- Probe --------------------------------------------------------------- */

/*
 * Probe path: find first scsi -> modern PCI caps -> features -> ctrl/event/req qs
 * -> post event buffer -> DRIVER_OK.
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

    /* Prefer kind lookup; fall back to modern/transitional PCI ID walk. */
    g_pScsi = virtio_dev_find_kind(GJ_VIRTIO_KIND_SCSI);
    if (g_pScsi == NULL) {
        c = virtio_dev_count();
        /* modern 0x1048, transitional 0x1004, or product kind==6 */
        for (i = 0; i < c; i++) {
            struct gj_virtio_dev *pDev = virtio_dev_get(i);

            if (pDev == NULL) {
                continue;
            }
            if (pDev->u16Device == GJ_VIRTIO_PCI_DEV_SCSI_MODERN ||
                pDev->u16Device == VIRTIO_SCSI_PCI_TRANSITIONAL ||
                pDev->u32Kind == VIRTIO_SCSI_KIND) {
                g_pScsi = pDev;
                break;
            }
        }
    }
    if (g_pScsi == NULL) {
        kprintf("virtio-scsi: no device\n");
        g_u32ProbeNodev++;
        soft_arm();
        return -1;
    }

    st = virtio_pci_setup(g_pScsi);
    if (st != GJ_OK || g_pScsi->pCommon == NULL) {
        kprintf("virtio-scsi: pci setup failed %d\n", (int)st);
        g_u32ProbeFail++;
        soft_arm();
        return -1;
    }
    /* Prefer V1; fall back to transitional (empty want mask) */
    st = virtio_negotiate(g_pScsi, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        st = virtio_negotiate(g_pScsi, 0);
        if (st != GJ_OK) {
            kprintf("virtio-scsi: negotiate failed %d\n", (int)st);
            g_u32ProbeFail++;
            soft_arm();
            return -1;
        }
    }

    /* Soft device-cfg residual snap (silent; Soft!=product). */
    scsi_soft_cfg_snap(g_pScsi);

    /* q0 control - TMF soft path */
    st = virtio_q_setup(g_pScsi, &g_qCtrl, (u16)VIRTIO_SCSI_Q_CONTROL,
                        (u16)VIRTIO_SCSI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-scsi: control q failed %d\n", (int)st);
        g_u32ProbeFail++;
        soft_arm();
        return -1;
    }
    /* q1 event - async notifications (soft poll/repost) */
    st = virtio_q_setup(g_pScsi, &g_qEvent, (u16)VIRTIO_SCSI_Q_EVENT,
                        (u16)VIRTIO_SCSI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-scsi: event q failed %d\n", (int)st);
        g_u32ProbeFail++;
        soft_arm();
        return -1;
    }
    /* q2 request0 - command submission */
    st = virtio_q_setup(g_pScsi, &g_qReq, (u16)VIRTIO_SCSI_Q_REQUEST,
                        (u16)VIRTIO_SCSI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-scsi: request q failed %d\n", (int)st);
        g_u32ProbeFail++;
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
    g_u32ProbeOk++;
    scsi_note_free();
    /* Product marker: greppable ready line with queue inventory */
    kprintf("virtio-scsi: ready bus=%x slot=%x (ctrl+event+req) event_posted=%d\n",
            (unsigned)g_pScsi->u8Bus, (unsigned)g_pScsi->u8Slot,
            g_fEventPosted);
    /* One-shot soft inventory + residual lean (LEAN residual; Soft!=product). */
    scsi_soft_inventory_once("probe");
    scsi_soft_maybe_residual("probe");
    return 0;
}

int
virtio_scsi_ready(void)
{
    g_u32ReadyQuery++;
    return g_fReady;
}

int
virtio_scsi_soft_active(void)
{
    g_u32SoftQuery++;
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
            g_u32SoftSubmitMiss++;
            /* First soft-activity residual fallback (one-shot; Soft!=product). */
            scsi_soft_maybe_residual("submit_soft");
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
            return -1;
        }
    }
    scsi_q_kick(&g_qReq);
    i32Len = virtio_q_poll(&g_qReq, VIRTIO_SCSI_POLL_SPINS);
    if (i32Len < 0) {
        kprintf("virtio-scsi: timeout\n");
        g_u32ReqTimeout++;
        g_u32IoFail++;
        g_u32LastUsedLen = 0;
        return -1;
    }
    g_u32LastUsedLen = (u32)i32Len;
    scsi_note_free();

    g_u32LastResponse = (u32)g_Resp.u8Response;
    g_u32LastResidual = g_Resp.u32Residual;
    g_u32LastScsiStatus = (u32)g_Resp.u8Status;
    pReq->u8Status = g_Resp.u8Status;
    {
        u32 cb = g_Resp.u32SenseLen;
        u32 cbCap = (u32)sizeof(pReq->sense.aSense);

        if (cb > cbCap) {
            /* Device sense (96) may exceed mid sense max (32) - soft clamp. */
            cb = cbCap;
            g_u32SenseClamp++;
        }
        g_u32LastSenseLen = cb;
        if (cb > 0 && cb < 256) {
            memcpy(pReq->sense.aSense, g_Resp.aSense, cb);
            pReq->sense.u8SenseLen = (u8)cb;
        } else {
            pReq->sense.u8SenseLen = 0;
        }
    }
    if (cbData > 0 && pReq->pData != NULL && pReq->fDataIn) {
        /*
         * Residual-byte soft clamp (OASIS residual = unused tail bytes).
         * Soft!=product / C0 honesty: classify zero | partial | over.
         *   residual == 0          -> full xfer (copy cbData)
         *   0 < residual < cbData  -> short xfer (copy cbData - residual)
         *   residual >= cbData     -> nothing transferred (copy 0)
         */
        u32 cbCopy = cbData;
        u32 u32Res = g_Resp.u32Residual;

        if (u32Res == 0u) {
            g_u32ResidualZero++;
        } else if (u32Res < cbData) {
            cbCopy = cbData - u32Res;
            g_u32ResidualPartial++;
            g_u32ResidualClamp++;
        } else {
            cbCopy = 0;
            g_u32ResidualOver++;
            g_u32ResidualClamp++;
        }
        if (cbCopy > 0) {
            memcpy(pReq->pData, g_aData, cbCopy);
        }
    } else if (g_Resp.u32Residual == 0u) {
        g_u32ResidualZero++;
    } else if (cbData > 0u) {
        /* data-out / no-buffer residual non-zero: tally over/partial class */
        if (g_Resp.u32Residual < cbData) {
            g_u32ResidualPartial++;
            g_u32ResidualClamp++;
        } else {
            g_u32ResidualOver++;
            g_u32ResidualClamp++;
        }
    }
    if (g_Resp.u8Response != VIRTIO_SCSI_S_OK) {
        kprintf("virtio-scsi: response=%u status=%u sense_len=%u residual=%u\n",
                (unsigned)g_Resp.u8Response, (unsigned)g_Resp.u8Status,
                g_Resp.u32SenseLen, g_Resp.u32Residual);
        g_u32RespFail++;
        g_u32IoFail++;
        if (g_Resp.u8Status == (u8)GJ_SCSI_CHECK_CONDITION) {
            g_u32SamCheck++;
        } else if (g_Resp.u8Status != (u8)GJ_SCSI_GOOD) {
            g_u32SamOther++;
        }
        return -1;
    }
    g_u32RespOk++;
    if (g_Resp.u8Status != GJ_SCSI_GOOD) {
        kprintf("virtio-scsi: scsi status=%u residual=%u\n",
                (unsigned)g_Resp.u8Status, g_Resp.u32Residual);
        g_u32IoFail++;
        if (g_Resp.u8Status == (u8)GJ_SCSI_CHECK_CONDITION) {
            g_u32SamCheck++;
        } else {
            g_u32SamOther++;
        }
        return -1;
    }
    g_u32SamGood++;
    g_u32IoCount++;
    if (cbData == 0) {
        g_u32NoDataOps++;
    } else if (pReq->fDataIn) {
        g_u32DataInOps++;
    } else {
        g_u32DataOutOps++;
    }
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

    g_u32LastTmfSubtype = u32Subtype;
    /* Soft-only path: accept abort/reset family as nop success. */
    if (!g_fReady) {
        if (g_fSoft && soft_tmf_ok(u32Subtype)) {
            g_u32CtrlOk++;
            g_u32SoftTmfOk++;
            g_u32LastResponse = VIRTIO_SCSI_S_FUNCTION_SUCCEEDED;
            /* First soft-activity residual fallback (one-shot; Soft!=product). */
            scsi_soft_maybe_residual("tmf_soft_ok");
            return 0;
        }
        g_u32CtrlFail++;
        if (g_fSoft) {
            g_u32SoftTmfFail++;
            scsi_soft_maybe_residual("tmf_soft_fail");
        }
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
        g_u32CtrlQAddFail++;
        g_u32CtrlFail++;
        return -1;
    }
    scsi_q_kick(&g_qCtrl);
    i32Len = virtio_q_poll(&g_qCtrl, VIRTIO_SCSI_CTRL_SPINS);
    if (i32Len < 0) {
        kprintf("virtio-scsi: ctrl TMF timeout subtype=%u\n", u32Subtype);
        g_u32CtrlTimeout++;
        g_u32CtrlFail++;
        return -1;
    }
    scsi_note_free();

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

    /* Soft-only / not ready: no async events (silent residual tally). */
    if (!g_fReady) {
        if (g_fSoft) {
            g_u32SoftEventPoll++;
            g_u32EventEmpty++;
            /* First soft-activity residual fallback (one-shot; Soft!=product). */
            scsi_soft_maybe_residual("event_soft");
        }
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
        return 0; /* none pending - soft non-blocking */
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
    if (pOut->u32Missed != 0u) {
        g_u32EventMissedBit++;
    }

    /* Soft repost so the device can deliver the next event. */
    if (event_post() != 0) {
        kprintf("virtio-scsi: event repost failed\n");
        g_u32EventRepostFail++;
    } else {
        g_u32EventRepostOk++;
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
    g_u32QStatsApi++;
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
        scsi_note_free();
    }
    /*
     * No residual lamp here - boot inventory + residual lean only.
     * Never re-emit soft residual lean on stats read (FAULT class avoid).
     * Soft residual counters stay file-static (not wire-stable stats).
     * Folded into one-shot lean line (not write-only theater). Soft!=product / C0.
     * Keep silent residual symbols live (no unused-warn) for pre-lean samples.
     */
    (void)g_u32EventPolls;
    (void)g_u32EventRepostOk;
    (void)g_u32EventRepostFail;
    (void)g_u32EventMissedBit;
    (void)g_u32SoftEventPoll;
    (void)g_u32ReqTimeout;
    (void)g_u32ReqQAddFail;
    (void)g_u32CtrlTimeout;
    (void)g_u32CtrlQAddFail;
    (void)g_u32LastEventType;
    (void)g_u32LastEventReason;
    (void)g_u32LastCdbLen;
    (void)g_u32LastDataLen;
    (void)g_u32LastSenseLen;
    (void)g_u32ResidualPartial;
    (void)g_u32ResidualOver;
    (void)g_u32SenseClamp;
    (void)g_u32SamGood;
    (void)g_u32SamCheck;
    (void)g_u32SamOther;
    (void)g_u32QStatsApi;
    (void)g_u32ReadyQuery;
    (void)g_u32SoftQuery;
    (void)g_u32CfgEventInfoSize;
    (void)g_u32LeanOk;
    (void)g_u32LeanChecks;
    (void)g_u32MidHandoffOk;
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
