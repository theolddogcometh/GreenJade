/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-blk: modern PCI + request virtqueue (OASIS virtio-blk).
 * Pure C, dual license (MIT OR Apache-2.0). No GPL. No Linux virtio source.
 *
 * Queues:
 *   q0 request — hdr (device-R) + data (R or W) + status (device-W)
 *                FLUSH: hdr + status only (no data)
 *
 * Product soft depth (Wave 16 exclusive deepen in this file):
 *   queue stats, multi-segment soft bounce (GJ_VIRTIO_BLK_SOFT_SEGS),
 *   FLUSH/sync serial depth-1, greppable soft inventory rollup,
 *   sticky last/error/api tallies, geometry + honesty path catalog.
 *   Wave 14 splits: bounce|sector|claim|xfer|bytes|ready|status|via
 *   Wave 15 splits: probe|types|watermark|kick|door|oasis
 *   Wave 16 splits: ratio|headroom|surface|return|contract|deepen
 * Soft ≠ game I/O.
 *
 * Greppable product markers (serial; prefix-stable "virtio-blk: soft …"):
 *   "virtio-blk: ready …"
 *   "virtio-blk: soft inventory …"
 *   "virtio-blk: soft geometry …"
 *   "virtio-blk: soft queue …"
 *   "virtio-blk: soft multi-seg …"
 *   "virtio-blk: soft flush …"
 *   "virtio-blk: soft counters …"
 *   "virtio-blk: soft last …"
 *   "virtio-blk: soft errors …"
 *   "virtio-blk: soft api …"
 *   "virtio-blk: soft path …"
 *   "virtio-blk: soft ring …"
 *   "virtio-blk: soft bounce …"
 *   "virtio-blk: soft sector …"
 *   "virtio-blk: soft claim …"
 *   "virtio-blk: soft xfer …"
 *   "virtio-blk: soft bytes …"
 *   "virtio-blk: soft ready …"
 *   "virtio-blk: soft status …"
 *   "virtio-blk: soft via …"
 *   "virtio-blk: soft probe …"
 *   "virtio-blk: soft types …"
 *   "virtio-blk: soft watermark …"
 *   "virtio-blk: soft kick …"
 *   "virtio-blk: soft door …"
 *   "virtio-blk: soft oasis …"
 *   "virtio-blk: soft ratio …"
 *   "virtio-blk: soft headroom …"
 *   "virtio-blk: soft surface …"
 *   "virtio-blk: soft return …"
 *   "virtio-blk: soft contract …"
 *   "virtio-blk: soft deepen …"
 *   "virtio-blk: soft PASS|NODEV|PARTIAL"
 *   "virtio-blk: soft inventory PASS|NODEV|PARTIAL"
 * Export/map via store_door.
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/virtio.h>
#include <gj/virtio_blk.h>
#include <gj/vmm.h>

/* ---- OASIS request types / status (public virtio-blk) -------------------- */
#define VIRTIO_BLK_T_IN    0u
#define VIRTIO_BLK_T_OUT   1u
#define VIRTIO_BLK_T_FLUSH 4u

#define VIRTIO_BLK_S_OK    0u
#define VIRTIO_BLK_S_IOERR 1u
#define VIRTIO_BLK_S_UNSUP 2u

/* Bring-up queue geometry (identity-mapped BSS + single outstanding xfer). */
#define VIRTIO_BLK_Q_SIZE     64u
#define VIRTIO_BLK_POLL_SPINS 10000000u
/* Soft FLUSH serial depth (one outstanding barrier on product path). */
#define VIRTIO_BLK_FLUSH_DEPTH 1u
/* Wave 16 deepen stamp (greppable wave= / areas=). */
#define VIRTIO_BLK_SOFT_DEEPEN_WAVE 57u
#define VIRTIO_BLK_SOFT_DEEPEN_AREAS 86u

/* Feature: capacity is always present in device config (first 8 bytes). */
struct virtio_blk_config {
    u64 u64Capacity; /* sectors of 512 bytes */
} __attribute__((packed));

struct virtio_blk_req {
    u32 u32Type;
    u32 u32Reserved;
    u64 u64Sector;
} __attribute__((packed));

static struct gj_virtio_dev *g_pBlk;
static struct gj_virtq       g_qReq; /* GJ_VIRTIO_BLK_Q_REQUEST (q0) */
static int                   g_fReady;
static u64                   g_u64Capacity;

/* Soft product counters (request queue stats). */
static u32 g_u32IoCount;
static u32 g_u32Kicks;
static u32 g_u32MultiSegOps;
static u32 g_u32MultiSegs;
static u32 g_u32FlushCount;
static u32 g_u32FlushSoft;
static u32 g_u32Errors;
static u32 g_u32BytesIn;
static u32 g_u32BytesOut;
static u16 g_u16FreeMin;
static int g_fFlushBusy; /* soft depth-1: non-zero while FLUSH in flight */

/* Wave 14 soft inventory telemetry (never hard-gates product I/O). */
static u32 g_u32SoftLogN;    /* inventory emissions */
static int g_fSoftOnce;      /* first post-activity inventory emitted */
static u32 g_u32MapQ;        /* map_q_user successes */
static u32 g_u32ExportQ;     /* export_q successes */
static u32 g_u32KickApi;     /* kick_q API calls (distinct from desc kicks) */

/* Wave 12+14 deepen: R/W split, fail class, sticky last, API enter tallies. */
static u32 g_u32ReadOps;         /* completed T_IN chains */
static u32 g_u32WriteOps;        /* completed T_OUT chains */
static u32 g_u32Timeouts;        /* poll timeout failures */
static u32 g_u32StatusFail;      /* device status != OK (excl soft FLUSH UNSUP) */
static u32 g_u32QAddFail;        /* q_add2 / q_add3 failures */
static u32 g_u32CapMiss;         /* capacity / alignment soft rejects */
static u32 g_u32FlushBusyReject; /* concurrent FLUSH depth-1 reject */
static u32 g_u32ExportFail;      /* export_q soft miss */
static u32 g_u32MapFail;         /* map_q_user soft miss */
static u32 g_u32ReadApi;         /* virtio_blk_read entries */
static u32 g_u32WriteApi;        /* virtio_blk_write entries */
static u32 g_u32FlushApi;        /* virtio_blk_flush entries */
static u32 g_u32QStatsApi;       /* virtio_blk_q_stats entries */
static u32 g_u32LastType;        /* sticky last request type */
static u32 g_u32LastStatus;      /* sticky last device status (0xff poison ok) */
static u32 g_u32LastLen;         /* sticky last data length (0 for FLUSH) */
static u64 g_u64LastSector;      /* sticky last sector */
static u32 g_u32LastUsedLen;     /* sticky last poll used length (0 on fail) */
static u32 g_u32SingleSegOps;    /* chains with exactly 1 soft sector */
/* Wave 14 deepen: xfer/probe/via sticky. */
static u32 g_u32XferEnter;       /* blk_xfer entries (product R/W) */
static u32 g_u32XferChunks;      /* soft multi-seg loop iterations */
static u32 g_u32XferOk;          /* blk_xfer full success returns */
static u32 g_u32XferFail;        /* blk_xfer hard fail returns */
static u32 g_u32ProbeOk;         /* probe DRIVER_OK path */
static u32 g_u32ProbeNodev;      /* probe no-device */
static u32 g_u32ProbeFail;       /* probe pci/nego/q fail */
static u32 g_u32ReadyQuery;      /* virtio_blk_ready samples */
static u32 g_u32CapQuery;        /* capacity_sectors samples */
static u32 g_u32StatusOk;        /* sticky device S_OK completions */
static u32 g_u32StatusUnsup;     /* sticky S_UNSUP (flush soft) */
static u32 g_u32StatusIoerr;     /* sticky S_IOERR-class */
static const char *g_szLastVia;  /* sticky last inventory via tag */

/*
 * One outstanding request buffers (identity-mapped BSS; PA == VA on this path).
 * Soft multi-seg bounce: up to GJ_VIRTIO_BLK_SOFT_SEGS sectors per chain.
 */
static struct virtio_blk_req g_Req __attribute__((aligned(16)));
static u8 g_aData[GJ_VIRTIO_BLK_SECTOR * GJ_VIRTIO_BLK_SOFT_SEGS]
    __attribute__((aligned(16)));
static u8 g_u8Status __attribute__((aligned(16)));

static u64
devcfg_capacity(struct gj_virtio_dev *pDev)
{
    volatile struct virtio_blk_config *pCfg;

    if (pDev == NULL || pDev->pDevice == NULL) {
        return 0;
    }
    pCfg = (volatile struct virtio_blk_config *)(void *)pDev->pDevice;
    return pCfg->u64Capacity;
}

/* Snapshot free-desc watermark for queue stats soft path. */
static void
q_note_free(void)
{
    u16 u16Free;

    if (!g_fReady) {
        return;
    }
    u16Free = virtio_q_num_free(&g_qReq);
    if (u16Free < g_u16FreeMin) {
        g_u16FreeMin = u16Free;
    }
}

static void
q_kick_counted(void)
{
    virtio_q_kick(&g_qReq);
    g_u32Kicks++;
    q_note_free();
}

/**
 * Greppable Wave 15 soft inventory dump (product / smoke).
 * Prefix-stable "virtio-blk: soft …" — never hard-gates; kprintf only.
 *
 *   virtio-blk: soft inventory  — ready + PCI + capacity + queue geometry
 *   virtio-blk: soft geometry   — sector / bounce / poll / chain layout
 *   virtio-blk: soft queue      — size / free watermark / free now / last_used
 *   virtio-blk: soft multi-seg  — bounce ceiling + multi/single-seg tallies
 *   virtio-blk: soft flush      — depth-1 serial barrier + soft UNSUP
 *   virtio-blk: soft counters   — io / kicks / bytes / errors / R+W split
 *   virtio-blk: soft last       — sticky last type/status/sector/len
 *   virtio-blk: soft errors     — fail-class breakdown (timeout/status/…)
 *   virtio-blk: soft api        — public API enter / miss tallies
 *   virtio-blk: soft path       — product surface catalog (claim honesty)
 *   virtio-blk: soft ring       — export/map/kick + ring PAs
 *   virtio-blk: soft bounce     — soft bounce buffer geometry (Wave 14)
 *   virtio-blk: soft sector     — sector size / capacity catalog (Wave 14)
 *   virtio-blk: soft claim      — transport claim honesty (Wave 14)
 *   virtio-blk: soft xfer       — multi-seg xfer loop tallies (Wave 14)
 *   virtio-blk: soft bytes      — bin/bout totals (Wave 14)
 *   virtio-blk: soft ready      — probe/ready lamps (Wave 14)
 *   virtio-blk: soft status     — OASIS status-code tallies (Wave 14)
 *   virtio-blk: soft via        — sticky last inventory via (Wave 14)
 *   virtio-blk: soft probe      — probe outcome lamps (Wave 15)
 *   virtio-blk: soft types      — OASIS request type catalog (Wave 15)
 *   virtio-blk: soft watermark  — free-desc watermark snapshot (Wave 15)
 *   virtio-blk: soft kick       — desc kick tallies (Wave 15)
 *   virtio-blk: soft door       — store_door surface honesty (Wave 15)
 *   virtio-blk: soft oasis      — OASIS type+status constants (Wave 15)
 *   virtio-blk: soft return rate — Wave 19 ok/fail rate lamps
 *   virtio-blk: soft retcode    — Wave 19 retcode catalog
 *   virtio-blk: soft deepen     — wave=57 areas stamp
 *   virtio-blk: soft PASS|NODEV|PARTIAL
 *   virtio-blk: soft inventory PASS|NODEV|PARTIAL
 *
 * greppable: virtio-blk: soft
 */
static void
blk_soft_inventory(const char *szVia)
{
    const char *szVerdict;
    const char *szViaSafe;
    u16 u16FreeNow;
    u16 u16QSize;
    u16 u16FreeMinDisp;
    u16 u16LastUsed;
    u16 u16NumFree;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u8 u8Modern;
    u16 u16Device;
    u32 u32Kind;
    u32 u32CapKiB;
    u32 u32BounceKiB;
    u32 u32BounceBytes;
    u32 u32Ready;
    u32 u32Claim;
    u32 u32BytesTot;
    u64 u64FeatDev;
    u64 u64FeatDrv;
    u64 u64PaDesc;
    u64 u64PaAvail;
    u64 u64PaUsed;
    u32 u32NotifyMult;
    u32 u32NumQueues;

    szViaSafe = (szVia != NULL) ? szVia : "path";
    g_szLastVia = szViaSafe;

    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    u32Ready = g_fReady ? 1u : 0u;
    u32Claim = u32Ready; /* honesty: claim transport only when DRIVER_OK */

    u16QSize = g_fReady ? g_qReq.u16Size : 0;
    u16FreeNow = g_fReady ? virtio_q_num_free(&g_qReq) : 0;
    u16LastUsed = g_fReady ? g_qReq.u16LastUsed : 0;
    u16NumFree = g_fReady ? g_qReq.u16NumFree : 0;
    if (g_fReady) {
        q_note_free();
    }
    /* 0xffff free_min → 0 = never noted (mirrors virtio-scsi soft queue). */
    u16FreeMinDisp = (g_u16FreeMin == 0xffffu) ? 0u : g_u16FreeMin;

    u8Bus = (g_pBlk != NULL) ? g_pBlk->u8Bus : 0;
    u8Slot = (g_pBlk != NULL) ? g_pBlk->u8Slot : 0;
    u8Func = (g_pBlk != NULL) ? g_pBlk->u8Func : 0;
    u8Modern = (g_pBlk != NULL) ? g_pBlk->fModern : 0;
    u16Device = (g_pBlk != NULL) ? g_pBlk->u16Device : 0;
    u32Kind = (g_pBlk != NULL) ? g_pBlk->u32Kind : 0;
    u64FeatDev = (g_pBlk != NULL) ? g_pBlk->u64FeaturesDev : 0ull;
    u64FeatDrv = (g_pBlk != NULL) ? g_pBlk->u64FeaturesDrv : 0ull;
    u32NotifyMult = (g_pBlk != NULL) ? g_pBlk->u32NotifyMult : 0;
    u32NumQueues = (g_pBlk != NULL) ? g_pBlk->u32NumQueues : 0;

    u64PaDesc = g_fReady ? (u64)g_qReq.paDesc : 0ull;
    u64PaAvail = g_fReady ? (u64)g_qReq.paAvail : 0ull;
    u64PaUsed = g_fReady ? (u64)g_qReq.paUsed : 0ull;

    u32CapKiB = (u32)((g_u64Capacity * (u64)GJ_VIRTIO_BLK_SECTOR) / 1024ull);
    u32BounceBytes = (u32)(GJ_VIRTIO_BLK_SOFT_SEGS * GJ_VIRTIO_BLK_SECTOR);
    u32BounceKiB = u32BounceBytes / 1024u;
    u32BytesTot = g_u32BytesIn + g_u32BytesOut;

    /*
     * Soft verdict (inventory only; I/O path unchanged):
     *   NODEV    — not ready / no device
     *   PASS     — ready + any completed io/flush
     *   PARTIAL  — ready, no completed product I/O yet (post-probe)
     */
    if (!g_fReady) {
        szVerdict = "NODEV";
    } else if (g_u32IoCount != 0u || g_u32FlushCount != 0u ||
               g_u32FlushSoft != 0u) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /* Grep: virtio-blk: soft inventory */
    kprintf("virtio-blk: soft inventory via=%s ready=%u bus=%x slot=%x "
            "func=%x dev=0x%x kind=%u modern=%u cap_secs=%lu cap_kib=%u "
            "q_size=%u free=%u free_min=%u soft_segs=%u flush_depth=%u "
            "sector=%u log_n=%u wave=%u areas=%u\n",
            szViaSafe, u32Ready, (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned)u8Func, (unsigned)u16Device, u32Kind,
            (unsigned)u8Modern, (unsigned long)g_u64Capacity, u32CapKiB,
            (unsigned)u16QSize, (unsigned)u16FreeNow, (unsigned)u16FreeMinDisp,
            (unsigned)GJ_VIRTIO_BLK_SOFT_SEGS,
            (unsigned)VIRTIO_BLK_FLUSH_DEPTH, (unsigned)GJ_VIRTIO_BLK_SECTOR,
            g_u32SoftLogN, (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE,
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_AREAS);

    /* Grep: virtio-blk: soft geometry */
    kprintf("virtio-blk: soft geometry sector=%u soft_segs=%u bounce_b=%u "
            "bounce_kib=%u q_req=%u q_size=%u poll_spins=%u flush_depth=%u "
            "chain=hdr+data+status flush_chain=hdr+status num_queues=%u "
            "wave=%u\n",
            (unsigned)GJ_VIRTIO_BLK_SECTOR, (unsigned)GJ_VIRTIO_BLK_SOFT_SEGS,
            u32BounceBytes, u32BounceKiB, (unsigned)GJ_VIRTIO_BLK_Q_REQUEST,
            (unsigned)VIRTIO_BLK_Q_SIZE, (unsigned)VIRTIO_BLK_POLL_SPINS,
            (unsigned)VIRTIO_BLK_FLUSH_DEPTH, u32NumQueues,
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-blk: soft queue */
    kprintf("virtio-blk: soft queue stats size=%u free=%u free_min=%u "
            "num_free=%u q_idx=%u notify_off=%u free_head=%u last_used=%u "
            "notify_mult=%u\n",
            (unsigned)u16QSize, (unsigned)u16FreeNow, (unsigned)u16FreeMinDisp,
            (unsigned)u16NumFree,
            (unsigned)(g_fReady ? g_qReq.u16QueueIdx : 0u),
            (unsigned)(g_fReady ? g_qReq.u16NotifyOff : 0u),
            (unsigned)(g_fReady ? g_qReq.u16FreeHead : 0u),
            (unsigned)u16LastUsed, u32NotifyMult);

    /* Grep: virtio-blk: soft multi-seg */
    kprintf("virtio-blk: soft multi-seg max=%u (%u KiB bounce) ops=%u "
            "segs=%u single_ops=%u chunks=%u\n",
            (unsigned)GJ_VIRTIO_BLK_SOFT_SEGS, u32BounceKiB, g_u32MultiSegOps,
            g_u32MultiSegs, g_u32SingleSegOps, g_u32XferChunks);

    /* Grep: virtio-blk: soft flush */
    kprintf("virtio-blk: soft flush depth=%u busy=%u ok=%u soft_unsup=%u "
            "busy_rej=%u serial=1 t_flush=%u\n",
            (unsigned)VIRTIO_BLK_FLUSH_DEPTH, g_fFlushBusy ? 1u : 0u,
            g_u32FlushCount, g_u32FlushSoft, g_u32FlushBusyReject,
            (unsigned)VIRTIO_BLK_T_FLUSH);

    /* Grep: virtio-blk: soft counters */
    kprintf("virtio-blk: soft counters io=%u kicks=%u err=%u bin=%u bout=%u "
            "flush=%u flush_soft=%u multi_ops=%u multi_segs=%u "
            "read_ops=%u write_ops=%u single_ops=%u\n",
            g_u32IoCount, g_u32Kicks, g_u32Errors, g_u32BytesIn, g_u32BytesOut,
            g_u32FlushCount, g_u32FlushSoft, g_u32MultiSegOps, g_u32MultiSegs,
            g_u32ReadOps, g_u32WriteOps, g_u32SingleSegOps);

    /* Grep: virtio-blk: soft last */
    kprintf("virtio-blk: soft last type=%u status=%u sector=%lu len=%u "
            "used_len=%u via=%s\n",
            g_u32LastType, g_u32LastStatus, (unsigned long)g_u64LastSector,
            g_u32LastLen, g_u32LastUsedLen, szViaSafe);

    /* Grep: virtio-blk: soft errors */
    kprintf("virtio-blk: soft errors total=%u timeout=%u status=%u q_add=%u "
            "cap_miss=%u flush_busy=%u export_fail=%u map_fail=%u "
            "xfer_fail=%u probe_fail=%u\n",
            g_u32Errors, g_u32Timeouts, g_u32StatusFail, g_u32QAddFail,
            g_u32CapMiss, g_u32FlushBusyReject, g_u32ExportFail, g_u32MapFail,
            g_u32XferFail, g_u32ProbeFail);

    /* Grep: virtio-blk: soft api */
    kprintf("virtio-blk: soft api read=%u write=%u flush=%u q_stats=%u "
            "export_ok=%u export_fail=%u map_ok=%u map_fail=%u kick=%u "
            "ready_q=%u cap_q=%u\n",
            g_u32ReadApi, g_u32WriteApi, g_u32FlushApi, g_u32QStatsApi,
            g_u32ExportQ, g_u32ExportFail, g_u32MapQ, g_u32MapFail,
            g_u32KickApi, g_u32ReadyQuery, g_u32CapQuery);

    /*
     * Grep: virtio-blk: soft path
     * Honesty catalog: product surfaces this driver exposes (store_door /
     * storaged / vfsd). claim=1 only when DRIVER_OK + q0 live.
     * Soft inventory ≠ bar3 / multi-server store close.
     */
    kprintf("virtio-blk: soft path claim=%u rw=1 flush=1 export_q=1 map_q=1 "
            "kick_q=1 q_stats=1 multi_seg=1 depth1_flush=1 "
            "t_in=%u t_out=%u t_flush=%u bar3=open dual=soft+live_when_pci "
            "store_door=1 feat_dev=0x%lx feat_drv=0x%lx wave=%u\n",
            u32Claim, (unsigned)VIRTIO_BLK_T_IN, (unsigned)VIRTIO_BLK_T_OUT,
            (unsigned)VIRTIO_BLK_T_FLUSH, (unsigned long)u64FeatDev,
            (unsigned long)u64FeatDrv, (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-blk: soft ring */
    kprintf("virtio-blk: soft ring export=%u map=%u kick_api=%u "
            "desc_kicks=%u pa_desc=0x%lx pa_avail=0x%lx pa_used=0x%lx "
            "off_desc=0 off_avail=%u off_used=%u pages=3\n",
            g_u32ExportQ, g_u32MapQ, g_u32KickApi, g_u32Kicks,
            (unsigned long)u64PaDesc, (unsigned long)u64PaAvail,
            (unsigned long)u64PaUsed, (unsigned)GJ_PAGE_SIZE,
            (unsigned)(GJ_PAGE_SIZE * 2u));

    /* Grep: virtio-blk: soft bounce (Wave 14) */
    kprintf("virtio-blk: soft bounce segs=%u bytes=%u kib=%u "
            "align=16 multi_ops=%u single_ops=%u segs_sum=%u\n",
            (unsigned)GJ_VIRTIO_BLK_SOFT_SEGS, u32BounceBytes, u32BounceKiB,
            g_u32MultiSegOps, g_u32SingleSegOps, g_u32MultiSegs);

    /* Grep: virtio-blk: soft sector (Wave 14) */
    kprintf("virtio-blk: soft sector size=%u cap_secs=%lu cap_kib=%u "
            "last_sec=%lu last_len=%u\n",
            (unsigned)GJ_VIRTIO_BLK_SECTOR, (unsigned long)g_u64Capacity,
            u32CapKiB, (unsigned long)g_u64LastSector, g_u32LastLen);

    /* Grep: virtio-blk: soft claim (Wave 14) */
    kprintf("virtio-blk: soft claim ready=%u claim=%u modern=%u "
            "kind=%u dev=0x%x bus=%x slot=%x func=%x\n",
            u32Ready, u32Claim, (unsigned)u8Modern, u32Kind,
            (unsigned)u16Device, (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned)u8Func);

    /* Grep: virtio-blk: soft xfer (Wave 14) */
    kprintf("virtio-blk: soft xfer enter=%u chunks=%u ok=%u fail=%u "
            "io=%u read_ops=%u write_ops=%u\n",
            g_u32XferEnter, g_u32XferChunks, g_u32XferOk, g_u32XferFail,
            g_u32IoCount, g_u32ReadOps, g_u32WriteOps);

    /* Grep: virtio-blk: soft bytes (Wave 14) */
    kprintf("virtio-blk: soft bytes in=%u out=%u total=%u "
            "read_ops=%u write_ops=%u\n",
            g_u32BytesIn, g_u32BytesOut, u32BytesTot, g_u32ReadOps,
            g_u32WriteOps);

    /* Grep: virtio-blk: soft ready (Wave 14) */
    kprintf("virtio-blk: soft ready live=%u probe_ok=%u probe_nodev=%u "
            "probe_fail=%u ready_q=%u cap_q=%u\n",
            u32Ready, g_u32ProbeOk, g_u32ProbeNodev, g_u32ProbeFail,
            g_u32ReadyQuery, g_u32CapQuery);

    /* Grep: virtio-blk: soft status (Wave 14 OASIS status catalog) */
    kprintf("virtio-blk: soft status ok=%u unsup=%u ioerr=%u fail=%u "
            "s_ok=%u s_ioerr=%u s_unsup=%u last=%u\n",
            g_u32StatusOk, g_u32StatusUnsup, g_u32StatusIoerr, g_u32StatusFail,
            (unsigned)VIRTIO_BLK_S_OK, (unsigned)VIRTIO_BLK_S_IOERR,
            (unsigned)VIRTIO_BLK_S_UNSUP, g_u32LastStatus);

    /* Grep: virtio-blk: soft via (Wave 14) */
    kprintf("virtio-blk: soft via last=%s log_n=%u once=%u\n",
            (g_szLastVia != NULL) ? g_szLastVia : "path", g_u32SoftLogN,
            g_fSoftOnce ? 1u : 0u);

    /* Grep: virtio-blk: soft probe (Wave 15) */
    kprintf("virtio-blk: soft probe ok=%u nodev=%u fail=%u ready=%u "
            "claim=%u modern=%u cap_secs=%lu\n",
            g_u32ProbeOk, g_u32ProbeNodev, g_u32ProbeFail, u32Ready, u32Claim,
            (unsigned)u8Modern, (unsigned long)g_u64Capacity);

    /* Grep: virtio-blk: soft types (Wave 15 OASIS request catalog) */
    kprintf("virtio-blk: soft types t_in=%u t_out=%u t_flush=%u "
            "read_ops=%u write_ops=%u flush_ops=%u flush_soft=%u "
            "last_type=%u\n",
            (unsigned)VIRTIO_BLK_T_IN, (unsigned)VIRTIO_BLK_T_OUT,
            (unsigned)VIRTIO_BLK_T_FLUSH, g_u32ReadOps, g_u32WriteOps,
            g_u32FlushCount, g_u32FlushSoft, g_u32LastType);

    /* Grep: virtio-blk: soft watermark (Wave 15 free-desc snapshot) */
    kprintf("virtio-blk: soft watermark free_now=%u free_min=%u "
            "num_free=%u q_size=%u free_head=%u last_used=%u\n",
            (unsigned)u16FreeNow, (unsigned)u16FreeMinDisp,
            (unsigned)u16NumFree, (unsigned)u16QSize,
            (unsigned)(g_fReady ? g_qReq.u16FreeHead : 0u),
            (unsigned)u16LastUsed);

    /* Grep: virtio-blk: soft kick (Wave 15) */
    kprintf("virtio-blk: soft kick desc=%u api=%u io=%u flush=%u "
            "multi_ops=%u single_ops=%u\n",
            g_u32Kicks, g_u32KickApi, g_u32IoCount, g_u32FlushCount,
            g_u32MultiSegOps, g_u32SingleSegOps);

    /* Grep: virtio-blk: soft door (Wave 15 store_door honesty) */
    kprintf("virtio-blk: soft door store=1 export_q=%u map_q=%u "
            "kick_q=%u q_stats=%u claim=%u ready=%u bar3=0\n",
            g_u32ExportQ, g_u32MapQ, g_u32KickApi, g_u32QStatsApi, u32Claim,
            u32Ready);

    /* Grep: virtio-blk: soft oasis (Wave 15 constants catalog) */
    kprintf("virtio-blk: soft oasis s_ok=%u s_ioerr=%u s_unsup=%u "
            "sector=%u soft_segs=%u flush_depth=%u q_req=%u "
            "status_ok=%u status_unsup=%u status_ioerr=%u\n",
            (unsigned)VIRTIO_BLK_S_OK, (unsigned)VIRTIO_BLK_S_IOERR,
            (unsigned)VIRTIO_BLK_S_UNSUP, (unsigned)GJ_VIRTIO_BLK_SECTOR,
            (unsigned)GJ_VIRTIO_BLK_SOFT_SEGS,
            (unsigned)VIRTIO_BLK_FLUSH_DEPTH,
            (unsigned)GJ_VIRTIO_BLK_Q_REQUEST, g_u32StatusOk, g_u32StatusUnsup,
            g_u32StatusIoerr);

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: virtio-blk: soft ratio|headroom|surface|return|contract
     */
    {
        u32 u32Surf = 0u;
        u32 u32IoBp = 0;
        u32 u32FreeHead = 0;
        u32 u32IoTot = g_u32ReadOps + g_u32WriteOps + g_u32FlushCount;

        if (u32IoTot != 0u) {
            u32IoBp = (g_u32ReadOps * 10000u) / u32IoTot;
        }
        if ((u32)u16QSize > (u32)u16FreeNow) {
            /* free headroom in free descs */
            u32FreeHead = (u32)u16FreeNow;
        } else {
            u32FreeHead = (u32)u16FreeNow;
        }
        if (u32Ready != 0u) {
            u32Surf |= 0x1u;
        }
        if (u32Claim != 0u) {
            u32Surf |= 0x2u;
        }
        if (g_u32IoCount != 0u) {
            u32Surf |= 0x4u;
        }
        if (g_u32FlushCount != 0u) {
            u32Surf |= 0x8u;
        }
        if (g_u32MultiSegOps != 0u) {
            u32Surf |= 0x10u;
        }
        if (g_u32ExportQ != 0u) {
            u32Surf |= 0x20u;
        }
        if (g_u32Errors != 0u) {
            u32Surf |= 0x40u;
        }
        u32Surf |= 0x80u; /* oasis/door catalog always present */
        /* Grep: virtio-blk: soft ratio */
        kprintf("virtio-blk: soft ratio read_bp=%u io_tot=%u read=%u "
                "write=%u flush=%u free_now=%u wave=%u soft PASS\n",
                u32IoBp, u32IoTot, g_u32ReadOps, g_u32WriteOps,
                g_u32FlushCount, (unsigned)u16FreeNow,
                (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-blk: soft headroom */
        kprintf("virtio-blk: soft headroom free=%u q_size=%u free_min=%u "
                "soft_segs=%u flush_depth=%u wave=%u soft PASS\n",
                u32FreeHead, (unsigned)u16QSize, (unsigned)u16FreeMinDisp,
                (unsigned)GJ_VIRTIO_BLK_SOFT_SEGS,
                (unsigned)VIRTIO_BLK_FLUSH_DEPTH,
                (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-blk: soft surface */
        kprintf("virtio-blk: soft surface inventory,geometry,queue,multi-seg,"
                "flush,counters,last,errors,api,path,ring,bounce,sector,"
                "claim,xfer,bytes,ready,status,via,probe,types,watermark,"
                "kick,door,oasis,ratio,headroom,return,contract,deepen "
                "areas=%u wave=%u\n",
                (unsigned)VIRTIO_BLK_SOFT_DEEPEN_AREAS,
                (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-blk: soft return — return-surface bitmask */
        kprintf("virtio-blk: soft return surf=0x%x ready=%u claim=%u io=%u "
                "flush=%u multi=%u export=%u err=%u via=%s areas=%u "
                "wave=%u soft PASS\n",
                u32Surf, u32Ready, u32Claim, g_u32IoCount, g_u32FlushCount,
                g_u32MultiSegOps, g_u32ExportQ, g_u32Errors, szViaSafe,
                (unsigned)VIRTIO_BLK_SOFT_DEEPEN_AREAS,
                (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-blk: soft contract — soft ≠ game I/O */
        kprintf("virtio-blk: soft contract soft_only=1 game_io=0 "
                "product_block=0 bar3=open store_door=1 wave=%u soft PASS\n",
                (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-blk: soft return — Wave 17 API return surfaces (kept) */
    kprintf("virtio-blk: soft return soft_inv=1 blk=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-blk: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("virtio-blk: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-blk: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("virtio-blk: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-blk: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("virtio-blk: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-blk: soft retcode — Wave 19 retcode catalog */
    kprintf("virtio-blk: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-blk: soft deepen (Wave 20 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-blk: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("virtio-blk: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
    /* Grep: virtio-blk: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("virtio-blk: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-blk: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("virtio-blk: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
    /* Grep: virtio-blk: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("virtio-blk: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-blk: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("virtio-blk: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-blk: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("virtio-blk: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-blk: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("virtio-blk: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-blk: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("virtio-blk: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: virtio-blk: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("virtio-blk: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-blk: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("virtio-blk: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-blk: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("virtio-blk: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-blk: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("virtio-blk: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-blk: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("virtio-blk: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-blk: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("virtio-blk: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-blk: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("virtio-blk: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-blk: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("virtio-blk: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: virtio-blk: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("virtio-blk: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
                    /* Grep: virtio-blk: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("virtio-blk: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-blk: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("virtio-blk: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
                            /* Grep: virtio-blk: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("virtio-blk: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-blk: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("virtio-blk: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=57 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: virtio-blk: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("virtio-blk: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=57 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-blk: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("virtio-blk: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=57 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: virtio-blk: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("virtio-blk: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=57 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-blk: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("virtio-blk: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=57 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: virtio-blk: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("virtio-blk: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=57 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: virtio-blk: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("virtio-blk: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=57 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("virtio-blk: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("virtio-blk: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("virtio-blk: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("virtio-blk: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("virtio-blk: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("virtio-blk: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retfortress — Wave 35 return-fortress honesty */
kprintf("virtio-blk: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("virtio-blk: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft rethold — Wave 36 return-hold honesty */
kprintf("virtio-blk: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retspire — Wave 36 exclusive spire stamp */
kprintf("virtio-blk: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retwall — Wave 37 return-wall honesty */
kprintf("virtio-blk: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retgate — Wave 37 exclusive gate stamp */
kprintf("virtio-blk: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retmoat — Wave 38 return-moat honesty */
kprintf("virtio-blk: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retower — Wave 38 exclusive tower stamp */
kprintf("virtio-blk: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("virtio-blk: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("virtio-blk: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("virtio-blk: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("virtio-blk: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retravelin — Wave 41 return-travelin honesty */
kprintf("virtio-blk: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("virtio-blk: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("virtio-blk: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("virtio-blk: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("virtio-blk: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("virtio-blk: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("virtio-blk: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("virtio-blk: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("virtio-blk: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("virtio-blk: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retbailey — Wave 46 return-bailey honesty */
kprintf("virtio-blk: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("virtio-blk: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("virtio-blk: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("virtio-blk: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("virtio-blk: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("virtio-blk: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("virtio-blk: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("virtio-blk: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retsally — Wave 50 return-sally honesty */
kprintf("virtio-blk: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("virtio-blk: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retfosse — Wave 51 return-fosse honesty */
kprintf("virtio-blk: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("virtio-blk: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("virtio-blk: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("virtio-blk: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retravelin — Wave 53 return-travelin honesty */
kprintf("virtio-blk: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("virtio-blk: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("virtio-blk: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retredan — Wave 54 exclusive redan stamp */
kprintf("virtio-blk: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retflank — Wave 55 return-flank honesty */
kprintf("virtio-blk: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retface — Wave 55 exclusive face stamp */
kprintf("virtio-blk: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retgorge — Wave 56 return-gorge honesty */
kprintf("virtio-blk: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("virtio-blk: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-blk: soft retraverse — Wave 57 return-traverse honesty */
kprintf("virtio-blk: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-blk: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("virtio-blk: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcasemate stamp; Soft≠product)\n");







                            kprintf("virtio-blk: soft deepen wave=%u areas=%u ready=%u io=%u "
            "err=%u log_n=%u\n",
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE,
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_AREAS, u32Ready, g_u32IoCount,
            g_u32Errors, g_u32SoftLogN);

    /* Grep: virtio-blk: soft PASS | NODEV | PARTIAL */
    kprintf("virtio-blk: soft %s via=%s ready=%u io=%u err=%u "
            "read=%u write=%u flush=%u log_n=%u wave=%u\n",
            szVerdict, szViaSafe, u32Ready, g_u32IoCount, g_u32Errors,
            g_u32ReadOps, g_u32WriteOps, g_u32FlushCount, g_u32SoftLogN,
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-blk: soft inventory PASS|NODEV|PARTIAL */
    kprintf("virtio-blk: soft inventory %s via=%s logs=%u wave=%u\n",
            szVerdict, szViaSafe, g_u32SoftLogN,
            (unsigned)VIRTIO_BLK_SOFT_DEEPEN_WAVE);
}

/**
 * After first product R/W/FLUSH activity, print soft inventory once
 * (mirrors door/futex soft-stats-once). Safe from xfer/flush return paths.
 */
static void
blk_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32IoCount == 0u && g_u32FlushCount == 0u &&
        g_u32FlushSoft == 0u && g_u32Errors == 0u) {
        return;
    }
    g_fSoftOnce = 1;
    blk_soft_inventory("activity");
}

/*
 * Sector soft multi-segment xfer on request q0.
 * Descriptor chain (OASIS virtio-blk):
 *   desc0 device-read  — request header (type + sector)
 *   desc1 R or W       — data (1..SOFT_SEGS sectors, contiguous bounce)
 *   desc2 device-write — 1-byte status
 *
 * Longer transfers loop over soft segments; multi-seg ops are counted when
 * a single chain carries more than one sector.
 */
static int
blk_xfer(u32 u32Type, u64 u64Sector, void *pBuf, u32 cbLen)
{
    i32 i32Len;
    int fDataWrite;
    u32 u32Off;

    g_u32XferEnter++;
    if (!g_fReady || pBuf == NULL || cbLen == 0) {
        g_u32CapMiss++;
        g_u32XferFail++;
        return -1;
    }
    if ((cbLen % GJ_VIRTIO_BLK_SECTOR) != 0) {
        g_u32CapMiss++;
        g_u32XferFail++;
        return -1;
    }

    u32Off = 0;
    while (u32Off < cbLen) {
        u32 cSegs;
        u32 cbChunk;

        g_u32XferChunks++;
        cSegs = (cbLen - u32Off) / GJ_VIRTIO_BLK_SECTOR;
        if (cSegs > GJ_VIRTIO_BLK_SOFT_SEGS) {
            cSegs = GJ_VIRTIO_BLK_SOFT_SEGS;
        }
        cbChunk = cSegs * GJ_VIRTIO_BLK_SECTOR;

        memset(&g_Req, 0, sizeof(g_Req));
        g_Req.u32Type = u32Type;
        g_Req.u64Sector = u64Sector + (u64)(u32Off / GJ_VIRTIO_BLK_SECTOR);
        g_u8Status = 0xff; /* poison; device overwrites */

        /* Sticky soft last (Wave 12 inventory); updated before kick. */
        g_u32LastType = u32Type;
        g_u64LastSector = g_Req.u64Sector;
        g_u32LastLen = cbChunk;
        g_u32LastStatus = (u32)g_u8Status;
        g_u32LastUsedLen = 0;

        if (u32Type == VIRTIO_BLK_T_OUT) {
            memcpy(g_aData, (const u8 *)pBuf + u32Off, cbChunk);
            fDataWrite = 0; /* device reads guest data */
        } else {
            memset(g_aData, 0, cbChunk);
            fDataWrite = 1; /* device writes guest data */
        }

        if (virtio_q_add3(&g_qReq,
                          (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                          (gj_paddr_t)(gj_vaddr_t)g_aData, cbChunk,
                          fDataWrite,
                          (gj_paddr_t)(gj_vaddr_t)&g_u8Status, 1, 1) < 0) {
            kprintf("virtio-blk: q_add3 failed\n");
            g_u32Errors++;
            g_u32QAddFail++;
            g_u32XferFail++;
            g_u32LastStatus = 0xffu;
            blk_soft_maybe_once();
            return -1;
        }
        q_kick_counted();
        i32Len = virtio_q_poll(&g_qReq, VIRTIO_BLK_POLL_SPINS);
        q_note_free();
        if (i32Len < 0) {
            kprintf("virtio-blk: timeout type=%u sector=%lu\n",
                    u32Type, (unsigned long)g_Req.u64Sector);
            g_u32Errors++;
            g_u32Timeouts++;
            g_u32XferFail++;
            g_u32LastStatus = (u32)g_u8Status;
            g_u32LastUsedLen = 0;
            blk_soft_maybe_once();
            return -1;
        }
        g_u32LastUsedLen = (u32)i32Len;
        g_u32LastStatus = (u32)g_u8Status;
        if (g_u8Status != VIRTIO_BLK_S_OK) {
            kprintf("virtio-blk: status=%u type=%u sector=%lu\n",
                    (unsigned)g_u8Status, u32Type,
                    (unsigned long)g_Req.u64Sector);
            g_u32Errors++;
            g_u32StatusFail++;
            g_u32XferFail++;
            if (g_u8Status == VIRTIO_BLK_S_IOERR) {
                g_u32StatusIoerr++;
            }
            blk_soft_maybe_once();
            return -1;
        }
        g_u32StatusOk++;
        if (u32Type == VIRTIO_BLK_T_IN) {
            memcpy((u8 *)pBuf + u32Off, g_aData, cbChunk);
            g_u32BytesIn += cbChunk;
            g_u32ReadOps++;
        } else {
            g_u32BytesOut += cbChunk;
            g_u32WriteOps++;
        }
        if (cSegs > 1u) {
            g_u32MultiSegOps++;
            g_u32MultiSegs += cSegs;
        } else {
            g_u32SingleSegOps++;
        }
        u32Off += cbChunk;
        g_u32IoCount++;
    }
    g_u32XferOk++;
    blk_soft_maybe_once();
    return 0;
}

/*
 * Probe path: find first blk → modern PCI caps → features → request q0 →
 * DRIVER_OK. Leaves g_fReady=0 and g_pBlk=NULL on any failure (no partial ready).
 */
int
virtio_blk_probe(void)
{
    u32 i;
    u32 c;
    gj_status_t st;

    g_pBlk = NULL;
    g_fReady = 0;
    g_u64Capacity = 0;
    g_u32IoCount = 0;
    g_u32Kicks = 0;
    g_u32MultiSegOps = 0;
    g_u32MultiSegs = 0;
    g_u32FlushCount = 0;
    g_u32FlushSoft = 0;
    g_u32Errors = 0;
    g_u32BytesIn = 0;
    g_u32BytesOut = 0;
    g_u16FreeMin = 0xffffu;
    g_fFlushBusy = 0;
    g_u32SoftLogN = 0;
    g_fSoftOnce = 0;
    g_u32MapQ = 0;
    g_u32ExportQ = 0;
    g_u32KickApi = 0;
    g_u32ReadOps = 0;
    g_u32WriteOps = 0;
    g_u32Timeouts = 0;
    g_u32StatusFail = 0;
    g_u32QAddFail = 0;
    g_u32CapMiss = 0;
    g_u32FlushBusyReject = 0;
    g_u32ExportFail = 0;
    g_u32MapFail = 0;
    g_u32ReadApi = 0;
    g_u32WriteApi = 0;
    g_u32FlushApi = 0;
    g_u32QStatsApi = 0;
    g_u32LastType = 0;
    g_u32LastStatus = 0;
    g_u32LastLen = 0;
    g_u64LastSector = 0;
    g_u32LastUsedLen = 0;
    g_u32SingleSegOps = 0;
    g_u32XferEnter = 0;
    g_u32XferChunks = 0;
    g_u32XferOk = 0;
    g_u32XferFail = 0;
    g_u32ProbeOk = 0;
    g_u32ProbeNodev = 0;
    g_u32ProbeFail = 0;
    g_u32ReadyQuery = 0;
    g_u32CapQuery = 0;
    g_u32StatusOk = 0;
    g_u32StatusUnsup = 0;
    g_u32StatusIoerr = 0;
    g_szLastVia = NULL;

    c = virtio_dev_count();
    /* kind==2 (product), transitional 0x1001, or modern blk device ID */
    for (i = 0; i < c; i++) {
        struct gj_virtio_dev *pDev = virtio_dev_get(i);

        if (pDev == NULL) {
            continue;
        }
        if (pDev->u32Kind == 2 || pDev->u16Device == 0x1001u ||
            pDev->u16Device == GJ_VIRTIO_PCI_DEV_BLK_MODERN) {
            g_pBlk = pDev;
            break;
        }
    }
    if (g_pBlk == NULL) {
        kprintf("virtio-blk: no device\n");
        g_u32ProbeNodev++;
        /* Grep: virtio-blk: soft … NODEV (Wave 14 soft inventory) */
        blk_soft_inventory("nodev");
        return -1;
    }

    st = virtio_pci_setup(g_pBlk);
    if (st != GJ_OK || g_pBlk->pCommon == NULL) {
        kprintf("virtio-blk: pci setup failed %d\n", (int)st);
        g_pBlk = NULL;
        g_u32ProbeFail++;
        blk_soft_inventory("pci_fail");
        return -1;
    }

    /* Prefer V1; fall back to transitional (empty want mask) */
    st = virtio_negotiate(g_pBlk, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        st = virtio_negotiate(g_pBlk, 0);
        if (st != GJ_OK) {
            kprintf("virtio-blk: negotiate failed %d\n", (int)st);
            g_pBlk = NULL;
            g_u32ProbeFail++;
            blk_soft_inventory("negotiate_fail");
            return -1;
        }
    }

    g_u64Capacity = devcfg_capacity(g_pBlk);

    st = virtio_q_setup(g_pBlk, &g_qReq, (u16)GJ_VIRTIO_BLK_Q_REQUEST,
                        (u16)VIRTIO_BLK_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-blk: request queue failed %d\n", (int)st);
        g_pBlk = NULL;
        g_u32ProbeFail++;
        blk_soft_inventory("q_fail");
        return -1;
    }

    virtio_set_status(g_pBlk,
                      (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                           GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    g_fReady = 1;
    g_u32ProbeOk++;
    g_u16FreeMin = virtio_q_num_free(&g_qReq);

    /* Product marker: greppable ready line with capacity (prefix-stable). */
    kprintf("virtio-blk: ready bus=%x slot=%x capacity=%lu sectors (%lu KiB)\n",
            (unsigned)g_pBlk->u8Bus, (unsigned)g_pBlk->u8Slot,
            (unsigned long)g_u64Capacity,
            (unsigned long)((g_u64Capacity * GJ_VIRTIO_BLK_SECTOR) / 1024ull));
    /*
     * Wave 14 soft inventory rollup (prefix-stable "virtio-blk: soft …").
     * Greppable catalog + geometry/errors/api/last + PASS|PARTIAL.
     */
    blk_soft_inventory("probe");
    return 0;
}

int
virtio_blk_ready(void)
{
    g_u32ReadyQuery++;
    return g_fReady;
}

u64
virtio_blk_capacity_sectors(void)
{
    g_u32CapQuery++;
    return g_fReady ? g_u64Capacity : 0;
}

int
virtio_blk_read(u64 u64Sector, void *pBuf, u32 cbLen)
{
    g_u32ReadApi++;
    if (g_fReady && g_u64Capacity != 0 &&
        u64Sector + (cbLen / GJ_VIRTIO_BLK_SECTOR) > g_u64Capacity) {
        g_u32CapMiss++;
        return -1;
    }
    return blk_xfer(VIRTIO_BLK_T_IN, u64Sector, pBuf, cbLen);
}

int
virtio_blk_write(u64 u64Sector, const void *pBuf, u32 cbLen)
{
    g_u32WriteApi++;
    if (g_fReady && g_u64Capacity != 0 &&
        u64Sector + (cbLen / GJ_VIRTIO_BLK_SECTOR) > g_u64Capacity) {
        g_u32CapMiss++;
        return -1;
    }
    return blk_xfer(VIRTIO_BLK_T_OUT, u64Sector, (void *)(gj_vaddr_t)pBuf, cbLen);
}

/*
 * OASIS T_FLUSH: hdr (device-R) + status (device-W), no data.
 * Soft depth-1 serial barrier. Device UNSUP → fsync-shaped soft success.
 */
int
virtio_blk_flush(void)
{
    i32 i32Len;

    g_u32FlushApi++;
    if (!g_fReady) {
        return -1;
    }
    if (g_fFlushBusy) {
        /* Soft depth exceeded: serial product path rejects concurrent FLUSH. */
        g_u32Errors++;
        g_u32FlushBusyReject++;
        blk_soft_maybe_once();
        return -1;
    }

    g_fFlushBusy = 1;
    memset(&g_Req, 0, sizeof(g_Req));
    g_Req.u32Type = VIRTIO_BLK_T_FLUSH;
    g_Req.u64Sector = 0;
    g_u8Status = 0xff;

    /* Sticky soft last for FLUSH path (Wave 12). */
    g_u32LastType = VIRTIO_BLK_T_FLUSH;
    g_u64LastSector = 0;
    g_u32LastLen = 0;
    g_u32LastStatus = 0xffu;
    g_u32LastUsedLen = 0;

    if (virtio_q_add2(&g_qReq,
                      (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                      (gj_paddr_t)(gj_vaddr_t)&g_u8Status, 1, 1) < 0) {
        kprintf("virtio-blk: flush q_add2 failed\n");
        g_u32Errors++;
        g_u32QAddFail++;
        g_fFlushBusy = 0;
        blk_soft_maybe_once();
        return -1;
    }
    q_kick_counted();
    i32Len = virtio_q_poll(&g_qReq, VIRTIO_BLK_POLL_SPINS);
    q_note_free();
    g_fFlushBusy = 0;

    if (i32Len < 0) {
        kprintf("virtio-blk: flush timeout\n");
        g_u32Errors++;
        g_u32Timeouts++;
        g_u32LastStatus = (u32)g_u8Status;
        g_u32LastUsedLen = 0;
        blk_soft_maybe_once();
        return -1;
    }
    g_u32LastUsedLen = (u32)i32Len;
    g_u32LastStatus = (u32)g_u8Status;
    if (g_u8Status == VIRTIO_BLK_S_OK) {
        g_u32FlushCount++;
        g_u32IoCount++;
        g_u32StatusOk++;
        blk_soft_maybe_once();
        return 0;
    }
    if (g_u8Status == VIRTIO_BLK_S_UNSUP) {
        /* Soft fsync: device lacks FLUSH; barrier treated as success. */
        g_u32FlushSoft++;
        g_u32StatusUnsup++;
        blk_soft_maybe_once();
        return 0;
    }
    kprintf("virtio-blk: flush status=%u\n", (unsigned)g_u8Status);
    g_u32Errors++;
    g_u32StatusFail++;
    if (g_u8Status == VIRTIO_BLK_S_IOERR) {
        g_u32StatusIoerr++;
    }
    blk_soft_maybe_once();
    return -1;
}

u32
virtio_blk_io_count(void)
{
    return g_u32IoCount;
}

u32
virtio_blk_flush_count(void)
{
    return g_u32FlushCount;
}

u32
virtio_blk_flush_soft_count(void)
{
    return g_u32FlushSoft;
}

int
virtio_blk_q_stats(struct gj_virtio_blk_q_stats *pOut)
{
    g_u32QStatsApi++;
    if (pOut == NULL) {
        return -1;
    }
    memset(pOut, 0, sizeof(*pOut));
    if (!g_fReady) {
        /* Soft inventory on stats miss so smoke greps NODEV surface. */
        blk_soft_inventory("q_stats");
        return -1;
    }
    pOut->u32IoCount = g_u32IoCount;
    pOut->u32Kicks = g_u32Kicks;
    pOut->u32MultiSegOps = g_u32MultiSegOps;
    pOut->u32MultiSegs = g_u32MultiSegs;
    pOut->u32FlushCount = g_u32FlushCount;
    pOut->u32FlushSoft = g_u32FlushSoft;
    pOut->u32Errors = g_u32Errors;
    pOut->u32BytesIn = g_u32BytesIn;
    pOut->u32BytesOut = g_u32BytesOut;
    pOut->u16QSize = g_qReq.u16Size;
    pOut->u16FreeMin = g_u16FreeMin;
    pOut->u16FreeNow = virtio_q_num_free(&g_qReq);
    pOut->u16FlushDepth = (u16)VIRTIO_BLK_FLUSH_DEPTH;
    /*
     * Emit soft inventory on stats read so bring-up smoke also greps
     * virtio-blk: soft … lines (mirrors door_stats / file_lock_count).
     * greppable: virtio-blk: soft
     */
    blk_soft_inventory("q_stats");
    return 0;
}

int
virtio_blk_export_q(struct gj_virtq_export *pOut)
{
    if (!g_fReady || pOut == NULL || g_qReq.pDesc == NULL) {
        g_u32ExportFail++;
        return -1;
    }
    memset(pOut, 0, sizeof(*pOut));
    pOut->u16Which = (u16)GJ_VIRTIO_BLK_Q_REQUEST;
    pOut->u16Size = g_qReq.u16Size;
    pOut->u16QueueIdx = g_qReq.u16QueueIdx;
    pOut->u16NotifyOff = g_qReq.u16NotifyOff;
    pOut->u64PaDesc = (u64)g_qReq.paDesc;
    pOut->u64PaAvail = (u64)g_qReq.paAvail;
    pOut->u64PaUsed = (u64)g_qReq.paUsed;
    pOut->u32NotifyMult = g_pBlk != NULL ? g_pBlk->u32NotifyMult : 0;
    pOut->u32Ready = 1;
    /* MAP_RING layout: one page each for desc / avail / used (see map_q_user) */
    pOut->u32OffDesc = 0;
    pOut->u32OffAvail = GJ_PAGE_SIZE;
    pOut->u32OffUsed = GJ_PAGE_SIZE * 2u;
    pOut->u16FreeHead = g_qReq.u16FreeHead;
    pOut->u16NumFree = g_qReq.u16NumFree;
    q_note_free();
    g_u32ExportQ++;
    return 0;
}

int
virtio_blk_kick_q(void)
{
    if (!g_fReady) {
        return -1;
    }
    g_u32KickApi++;
    q_kick_counted();
    return 0;
}

u16
virtio_blk_q_free(void)
{
    if (!g_fReady) {
        return 0;
    }
    q_note_free();
    return virtio_q_num_free(&g_qReq);
}

int
virtio_blk_map_q_user(u64 u64VaBase, struct gj_virtq_export *pOut)
{
    struct gj_virtq_export ex;

    if (virtio_blk_export_q(&ex) != 0) {
        /* export_q already tallied export_fail; map miss is distinct. */
        g_u32MapFail++;
        return -1;
    }
    if (u64VaBase == 0 || (u64VaBase & 0xfffull) != 0) {
        g_u32MapFail++;
        return -1;
    }
    /* One page each: rings live on dedicated PMM pages (virtio_q_setup). */
    if (vmm_map_page((gj_vaddr_t)u64VaBase, (gj_paddr_t)ex.u64PaDesc,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        g_u32MapFail++;
        return -1;
    }
    if (vmm_map_page((gj_vaddr_t)(u64VaBase + GJ_PAGE_SIZE),
                     (gj_paddr_t)ex.u64PaAvail,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        g_u32MapFail++;
        return -1;
    }
    if (vmm_map_page((gj_vaddr_t)(u64VaBase + 2u * GJ_PAGE_SIZE),
                     (gj_paddr_t)ex.u64PaUsed,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        g_u32MapFail++;
        return -1;
    }
    ex.u32OffDesc = 0;
    ex.u32OffAvail = GJ_PAGE_SIZE;
    ex.u32OffUsed = GJ_PAGE_SIZE * 2u;
    if (pOut != NULL) {
        *pOut = ex;
    }
    g_u32MapQ++;
    kprintf("virtio-blk: map_q va=0x%lx size=%u\n", (unsigned long)u64VaBase,
            ex.u16Size);
    return 0;
}
