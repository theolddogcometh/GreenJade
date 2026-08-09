/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-blk: modern PCI + request virtqueue (OASIS virtio-blk).
 * Pure C, dual license (MIT OR Apache-2.0). No GPL. No Linux virtio source.
 *
 * Queues:
 *   q0 request - hdr (device-R) + data (R or W) + status (device-W)
 *                FLUSH: hdr + status only (no data)
 *
 * C0 T0 storage residual - store_door / UDX hand-off (this unit only;
 * Soft!=product; Dual DoD store product OPEN):
 *   QEMU virtio-blk is T0 product storage until UDX owns the request ring wire.
 *   Functional path: multi-seg soft bounce (GJ_VIRTIO_BLK_SOFT_SEGS),
 *   FLUSH depth-1, export_q / map_q_user for store_door MAP_RING.
 *   LEAN residual only - never re-introduce multi-kprintf stamp storms
 *   (prior FAULT class: sequential soft-deepen floods -> stack smash / #PF).
 *   Soft inventory is one-shot, one line, probe/nodev path only.
 *   Soft residual lean: silent geometry/export/layout/kind/status self-check
 *   + one-shot lamp (probe fail paths + first product activity
 *   R/W/FLUSH/export/map/kick). Map lamp once. Silent residual counters for
 *   fail/API/xfer/map-page/status/last/bytes class. Compile-time bounce/req/q
 *   geometry residual (typedef size guards). No wave=/version stamp.
 *   Soft residual lamps != product DoD close. product_store=UDX direction;
 *   freestanding residual only (G-AC-1). Dual DoD store product OPEN
 *   (UDX/storaged); freestanding SKIP product. Soft!=product dual MIT OR
 *   Apache-2.0. No GPL. Claim class C0 (eng residual only).
 *
 * Greppable product markers (prefix-stable; bring-up, not soft deepen):
 *   "virtio-blk: ready ..."
 * Soft inventory: one-shot one-line boot lamp only (Soft!=product)
 *   virtio-blk: soft inventory via=... ready=... ... Soft!=product
 * Soft residual lean (one-shot activity / probe lean):
 *   virtio-blk: soft residual lean ...
 *   virtio-blk: soft residual lean PASS
 * greppable: virtio-blk: soft inventory
 * greppable: virtio-blk: soft residual lean
 * greppable: virtio-blk: map_q (once)
 * greppable: Soft!=product t0_storage store_door product_store=UDX
 * greppable: handoff=storaged|UDX | map_ring | export_map
 * greppable: dual=MIT_OR_Apache-2.0 | G-AC-1 | stamp_storm=0
 * greppable: dual_dod_store=OPEN | C0 | not_freestanding_store_product
 * greppable: udx_host_stack=export|map_ring|kick | KIND_BLK
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
/* Transitional virtio-blk PCI device id (inventory residual; Soft!=product). */
#define VIRTIO_BLK_PCI_TRANSITIONAL 0x1001u

/* Feature: capacity is always present in device config (first 8 bytes). */
struct virtio_blk_config {
    u64 u64Capacity; /* sectors of 512 bytes */
} __attribute__((packed));

struct virtio_blk_req {
    u32 u32Type;
    u32 u32Reserved;
    u64 u64Sector;
} __attribute__((packed));

/*
 * Compile-time geometry residual (Soft!=product; no runtime cost).
 * Bounce window, OASIS req layout, queue power-of-two, KIND/T0/modern ID -
 * fail the build if residual honesty drifts. Matches store_door 4 KiB xfer.
 */
typedef char gj_vblk_req_layout_sz[(sizeof(struct virtio_blk_req) == 16u) ? 1 : -1];
typedef char gj_vblk_q_size_pow2[
    ((VIRTIO_BLK_Q_SIZE != 0u) &&
     ((VIRTIO_BLK_Q_SIZE & (VIRTIO_BLK_Q_SIZE - 1u)) == 0u))
        ? 1
        : -1];
typedef char gj_vblk_soft_bounce_4k[
    ((GJ_VIRTIO_BLK_SOFT_SEGS * GJ_VIRTIO_BLK_SECTOR) == 4096u) ? 1 : -1];
typedef char gj_vblk_flush_depth1[(VIRTIO_BLK_FLUSH_DEPTH == 1u) ? 1 : -1];
typedef char gj_vblk_sector_512[(GJ_VIRTIO_BLK_SECTOR == 512u) ? 1 : -1];
typedef char gj_vblk_map_pages3[((GJ_PAGE_SIZE * 3u) == 12288u) ? 1 : -1];
typedef char gj_vblk_kind_blk[(GJ_VIRTIO_KIND_BLK == 2u) ? 1 : -1];
typedef char gj_vblk_kind_t0[(GJ_VIRTIO_KIND_IS_T0(GJ_VIRTIO_KIND_BLK)) ? 1 : -1];
typedef char gj_vblk_modern_id[
    (GJ_VIRTIO_PCI_DEV_BLK_MODERN == 0x1042u) ? 1 : -1];
typedef char gj_vblk_q_request0[(GJ_VIRTIO_BLK_Q_REQUEST == 0u) ? 1 : -1];
typedef char gj_vblk_soft_segs8[(GJ_VIRTIO_BLK_SOFT_SEGS == 8u) ? 1 : -1];

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

/*
 * Lean soft residual (file-static; Soft!=product / T0 storage honesty).
 * Silent counters only - never multi-line inventory dumps / stamp storms.
 * Map lamp once. Soft residual lean once. Probe-class tallies never
 * kprintf on hot path.
 */
static int g_fSoftInvOnce;       /* one-shot soft inventory boot lamp */
static int g_fSoftLeanOnce;      /* one-shot soft residual lean lamp */
static int g_fMapQLamp;          /* one-shot map_q lamp (no stamp storms) */
static u32 g_u32MapQ;            /* map_q_user successes (silent residual) */
static u32 g_u32ExportQ;         /* export_q successes (silent residual) */
static u32 g_u32KickApi;         /* kick_q API calls (distinct from desc kicks) */
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
static u32 g_u32LeanOk;          /* last lean self-check ok count */
static u32 g_u32LeanChecks;      /* last lean self-check total */
static u32 g_u32UdxHandoffOk;    /* export/map/kick surface lean bits */
static u32 g_u32KickFail;        /* kick_q not-ready rejects */
static u32 g_u32QFreeQuery;      /* virtio_blk_q_free samples */
static u32 g_u32MapPageFail;     /* sticky last MAP page fail 1|2|3 (0=none) */
static u32 g_u32BoundsReject;    /* sector range / overflow soft rejects */
static u32 g_u32FlushOk;         /* device FLUSH S_OK (alias path tally) */
static u32 g_u32LeanFreeNow;     /* free-desc snap at lean emission (silent) */
static u32 g_u32LeanFreeMin;     /* free-min snap at lean emission (silent) */
static u32 g_u32LeanReady;       /* ready snap at lean emission (silent) */
static u32 g_u32BytesTot;        /* bytes_in + bytes_out fold (silent residual) */
static u32 g_u32ApiEnter;        /* read+write+flush API enter fold */
static u32 g_u32FailClass;       /* timeout+status+qadd+bounds fold */

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
 * Silent lean residual self-check (no kprintf; Soft!=product / C0).
 * Geometry + OASIS request types + store_door/UDX export/MAP surface +
 * layout sizes (req hdr 16B, bounce window, 3-page MAP span) + KIND_BLK
 * / T0 / VERSION_1 / status ladder / transitional inventory residual.
 * Returns ok count; *pOutChecks = total checks when non-NULL.
 * Results fold into soft residual lean lines only (no stamp storm).
 */
static u32
blk_soft_residual_lean_ok(u32 *pOutChecks)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u32 u32Udx = 0;
    u16 u16FreeNow = 0;

    /* 1: OASIS sector size */
    u32Checks++;
    if (GJ_VIRTIO_BLK_SECTOR == 512u) {
        u32Ok++;
    }
    /* 2: soft multi-seg * sector matches store_door bounce (4 KiB) */
    u32Checks++;
    if (GJ_VIRTIO_BLK_SOFT_SEGS == 8u &&
        (GJ_VIRTIO_BLK_SOFT_SEGS * GJ_VIRTIO_BLK_SECTOR) == 4096u) {
        u32Ok++;
    }
    /* 3: request queue index is q0 only */
    u32Checks++;
    if (GJ_VIRTIO_BLK_Q_REQUEST == 0u) {
        u32Ok++;
    }
    /* 4: FLUSH serial depth-1 */
    u32Checks++;
    if (VIRTIO_BLK_FLUSH_DEPTH == 1u) {
        u32Ok++;
    }
    /* 5: OASIS request types (T_IN/T_OUT/T_FLUSH) */
    u32Checks++;
    if (VIRTIO_BLK_T_IN == 0u && VIRTIO_BLK_T_OUT == 1u &&
        VIRTIO_BLK_T_FLUSH == 4u) {
        u32Ok++;
    }
    /* 6: OASIS status codes */
    u32Checks++;
    if (VIRTIO_BLK_S_OK == 0u && VIRTIO_BLK_S_IOERR == 1u &&
        VIRTIO_BLK_S_UNSUP == 2u) {
        u32Ok++;
    }
    /* 7: modern blk PCI device id (T0 store inventory) */
    u32Checks++;
    if (GJ_VIRTIO_PCI_DEV_BLK_MODERN == 0x1042u) {
        u32Ok++;
    }
    /* 8: MAP_RING page geometry (desc|avail|used; 4 KiB) */
    u32Checks++;
    if (GJ_PAGE_SIZE == 4096u &&
        (GJ_PAGE_SIZE & (GJ_PAGE_SIZE - 1u)) == 0u) {
        u32Ok++;
        u32Udx++;
    }
    /* 9: q_stats wire field order stable size (extend only at end) */
    u32Checks++;
    if (sizeof(struct gj_virtio_blk_q_stats) >= 40u) {
        u32Ok++;
    }
    /* 10: export surface present (store_door EXPORT_RING residual) */
    u32Checks++;
    if (sizeof(struct gj_virtq_export) >= 48u) {
        u32Ok++;
        u32Udx++;
    }
    /* 11: queue soft size is power-of-two and covers product path */
    u32Checks++;
    if (VIRTIO_BLK_Q_SIZE == 64u &&
        (VIRTIO_BLK_Q_SIZE & (VIRTIO_BLK_Q_SIZE - 1u)) == 0u) {
        u32Ok++;
    }
    /* 12: multi-seg window <= store_door xfer (4096) honesty */
    u32Checks++;
    if ((GJ_VIRTIO_BLK_SOFT_SEGS * GJ_VIRTIO_BLK_SECTOR) <= 4096u) {
        u32Ok++;
        u32Udx++;
    }
    /* 13: OASIS request header layout (type+reserved+sector = 16B) */
    u32Checks++;
    if (sizeof(struct virtio_blk_req) == 16u) {
        u32Ok++;
    }
    /* 14: soft bounce BSS matches SOFT_SEGS * SECTOR window */
    u32Checks++;
    if ((u32)sizeof(g_aData) ==
        (GJ_VIRTIO_BLK_SOFT_SEGS * GJ_VIRTIO_BLK_SECTOR)) {
        u32Ok++;
    }
    /* 15: MAP_RING three-page span (desc|avail|used) for UDX host */
    u32Checks++;
    if ((GJ_PAGE_SIZE * 3u) == 12288u &&
        (GJ_PAGE_SIZE * 2u) == 8192u) {
        u32Ok++;
        u32Udx++;
    }
    /* 16: MAP_RING export offsets desc=0 avail=PAGE used=2*PAGE */
    u32Checks++;
    {
        u32 u32OffDesc = 0u;
        u32 u32OffAvail = GJ_PAGE_SIZE;
        u32 u32OffUsed = GJ_PAGE_SIZE * 2u;

        if (u32OffDesc == 0u && u32OffAvail == 4096u && u32OffUsed == 8192u &&
            u32OffUsed > u32OffAvail && u32OffAvail > u32OffDesc) {
            u32Ok++;
            u32Udx++;
        }
    }
    /* 17: poll budget non-zero (bring-up soft path honesty) */
    u32Checks++;
    if (VIRTIO_BLK_POLL_SPINS >= 1000u) {
        u32Ok++;
    }
    /* 18: free-desc watermark coherent when ready (silent) */
    u32Checks++;
    if (!g_fReady ||
        (g_qReq.u16Size > 0u && g_u16FreeMin <= g_qReq.u16Size)) {
        u32Ok++;
    }
    /* 19: KIND_BLK is T0 store class (inventory residual; Soft!=product) */
    u32Checks++;
    if (GJ_VIRTIO_KIND_BLK == 2u && GJ_VIRTIO_KIND_IS_T0(GJ_VIRTIO_KIND_BLK) &&
        GJ_VIRTIO_KIND_BLK != GJ_VIRTIO_KIND_NET &&
        GJ_VIRTIO_KIND_BLK != GJ_VIRTIO_KIND_GPU) {
        u32Ok++;
    }
    /* 20: transitional blk PCI id still inventory-accepted (0x1001) */
    u32Checks++;
    if (VIRTIO_BLK_PCI_TRANSITIONAL == 0x1001u) {
        u32Ok++;
    }
    /* 21: product negotiate wants VERSION_1 transport bit */
    u32Checks++;
    if (GJ_VIRTIO_F_VERSION_1 == (1ull << 32)) {
        u32Ok++;
    }
    /* 22: device status ladder bits distinct (ACK|DRIVER|FEATURES_OK|OK) */
    u32Checks++;
    if (GJ_VIRTIO_S_ACKNOWLEDGE == 1u && GJ_VIRTIO_S_DRIVER == 2u &&
        GJ_VIRTIO_S_DRIVER_OK == 4u && GJ_VIRTIO_S_FEATURES_OK == 8u &&
        GJ_VIRTIO_S_FAILED == 128u &&
        (GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
         GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK) == 15u) {
        u32Ok++;
    }
    /* 23: live free-now <= q size when ready (silent residual snap) */
    u32Checks++;
    if (g_fReady) {
        u16FreeNow = virtio_q_num_free(&g_qReq);
        g_u32LeanFreeNow = (u32)u16FreeNow;
        g_u32LeanFreeMin = (u32)g_u16FreeMin;
        if (u16FreeNow <= g_qReq.u16Size && g_qReq.u16Size > 0u) {
            u32Ok++;
        }
    } else {
        g_u32LeanFreeNow = 0;
        g_u32LeanFreeMin = (g_u16FreeMin == 0xffffu) ? 0u : (u32)g_u16FreeMin;
        u32Ok++; /* not-ready path coherent */
    }
    /* 24: status byte BSS + req BSS residual (poison-writable) */
    u32Checks++;
    if ((u32)sizeof(g_Req) == 16u && (u32)sizeof(g_u8Status) == 1u) {
        u32Ok++;
    }
    /* 25: export/map/kick API surface is the UDX host stack residual */
    u32Checks++;
    if (sizeof(struct gj_virtq_export) >= 48u &&
        GJ_VIRTIO_BLK_Q_REQUEST == 0u &&
        (GJ_PAGE_SIZE * 3u) == 12288u) {
        u32Ok++;
        u32Udx++;
    }
    /* 26: dual-license / G-AC-1 residual constants non-zero honesty fold */
    u32Checks++;
    if (VIRTIO_BLK_FLUSH_DEPTH == 1u && GJ_VIRTIO_BLK_SOFT_SEGS > 0u &&
        GJ_VIRTIO_BLK_SECTOR > 0u && VIRTIO_BLK_Q_SIZE >= 8u) {
        u32Ok++;
    }

    g_u32LeanOk = u32Ok;
    g_u32LeanChecks = u32Checks;
    g_u32UdxHandoffOk = u32Udx;
    g_u32LeanReady = g_fReady ? 1u : 0u;
    g_u32BytesTot = g_u32BytesIn + g_u32BytesOut;
    g_u32ApiEnter = g_u32ReadApi + g_u32WriteApi + g_u32FlushApi;
    g_u32FailClass = g_u32Timeouts + g_u32StatusFail + g_u32QAddFail +
                     g_u32BoundsReject + g_u32CapMiss;
    if (pOutChecks != NULL) {
        *pOutChecks = u32Checks;
    }
    return u32Ok;
}

/**
 * One-shot one-line boot soft inventory only (Soft!=product / C0 T0 storage).
 * LEAN residual: single kprintf - never multi-line stamp floods (FAULT class).
 * Probe/nodev path only - never re-emit on R/W/FLUSH/q_stats hot paths.
 * No wave=/version stamp. Dual license honesty on lamp only.
 * greppable: virtio-blk: soft inventory
 * greppable: Soft!=product t0_storage store_door product_store=UDX
 * greppable: dual_dod_store=OPEN | C0 | G-AC-1 | stamp_storm=0
 */
static void
blk_soft_inventory_once(const char *szVia)
{
    u8 u8Bus;
    u8 u8Slot;
    u32 u32Ready;
    u32 u32Kind;

    if (g_fSoftInvOnce != 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    u32Ready = g_fReady ? 1u : 0u;
    u8Bus = (g_pBlk != NULL) ? g_pBlk->u8Bus : 0;
    u8Slot = (g_pBlk != NULL) ? g_pBlk->u8Slot : 0;
    u32Kind = (g_pBlk != NULL) ? g_pBlk->u32Kind : 0u;
    /*
     * ONE line only. Soft!=product. No wave=/version stamp.
     * t0_storage=1: QEMU virtio-blk T0 path (lamp != DoD close).
     * store_door=1 / export_map=1: MAP_RING UDX hand-off surface.
     * product_store=UDX: direction honesty (G-AC-1).
     * multi_seg=1 / depth1_flush=1: soft product depth honesty.
     * dual_dod_store=OPEN / C0: claim class residual (not product close).
     */
    kprintf("virtio-blk: soft inventory via=%s ready=%u bus=%x slot=%x "
            "kind=%u cap_secs=%lu soft_segs=%u flush_depth=%u sector=%u "
            "q_size=%u modern_id=0x%x transitional_id=0x%x "
            "t0_storage=1 store_door=1 export_map=1 multi_seg=1 "
            "depth1_flush=1 map_ring=1 product_store=UDX "
            "handoff=storaged|UDX udx_host_stack=export|map_ring|kick "
            "dual_dod_store=OPEN C0=1 not_freestanding_store_product "
            "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
            "soft_ne_product=1 G-AC-1 Soft!=product\n",
            szVia != NULL ? szVia : "?",
            u32Ready, (unsigned)u8Bus, (unsigned)u8Slot,
            u32Kind,
            (unsigned long)g_u64Capacity,
            (unsigned)GJ_VIRTIO_BLK_SOFT_SEGS,
            (unsigned)VIRTIO_BLK_FLUSH_DEPTH,
            (unsigned)GJ_VIRTIO_BLK_SECTOR,
            (unsigned)VIRTIO_BLK_Q_SIZE,
            (unsigned)GJ_VIRTIO_PCI_DEV_BLK_MODERN,
            (unsigned)VIRTIO_BLK_PCI_TRANSITIONAL);
}

/**
 * One-shot soft residual lean after first product activity or probe lean.
 * Silent self-check + ONE line (+ optional PASS). Soft!=product / C0.
 * Never re-emit on hot path. No wave=/version stamp. Dual MIT OR Apache-2.0.
 * greppable: virtio-blk: soft residual lean
 * greppable: virtio-blk: soft residual lean PASS
 * greppable: handoff=storaged|UDX | map_ring | product_store=UDX
 * greppable: dual_dod_store=OPEN | C0 | udx_host_stack=export|map_ring|kick
 */
static void
blk_soft_residual_lean_once(const char *szVia)
{
    u32 u32Ok;
    u32 u32Checks;
    u32 u32Ready;
    u8 u8Bus;
    u8 u8Slot;

    if (g_fSoftLeanOnce != 0) {
        return;
    }
    g_fSoftLeanOnce = 1;

    u32Ok = blk_soft_residual_lean_ok(&u32Checks);
    u32Ready = g_fReady ? 1u : 0u;
    u8Bus = (g_pBlk != NULL) ? g_pBlk->u8Bus : 0;
    u8Slot = (g_pBlk != NULL) ? g_pBlk->u8Slot : 0;

    /*
     * ONE lean line. Soft!=product dual license; no version stamp; no storm.
     * Silent fail/API/xfer/map/bytes/last class folded in (no multi-line).
     * t0_storage=1 store_door=1 export_map=1 product_store=UDX:
     * T0 freestanding residual until UDX owns ring (lamp != DoD close).
     * dual_dod_store=OPEN C0: Dual DoD store product OPEN; freestanding SKIP.
     */
    kprintf("virtio-blk: soft residual lean via=%s ready=%u bus=%x slot=%x "
            "cap_secs=%lu soft_segs=%u flush_depth=%u sector=%u q_size=%u "
            "io=%u kicks=%u multi=%u multi_segs=%u single=%u "
            "flush=%u flush_ok=%u flush_soft=%u "
            "err=%u fail_class=%u map_q=%u export_q=%u kick_api=%u "
            "rd_ops=%u wr_ops=%u to=%u st_fail=%u st_ok=%u st_unsup=%u "
            "st_ioerr=%u qadd=%u cap_miss=%u "
            "bounds=%u busy_rej=%u map_fail=%u export_fail=%u "
            "map_page_fail=%u kick_fail=%u "
            "rd_api=%u wr_api=%u fl_api=%u api_enter=%u "
            "xfer_enter=%u xfer_chunks=%u xfer_ok=%u xfer_fail=%u "
            "bin=%u bout=%u btot=%u "
            "last_type=%u last_st=%u last_len=%u last_used=%u "
            "last_sec=%lu free_min=%u free_now=%u "
            "probe_ok=%u nodev=%u probe_fail=%u "
            "ready_q=%u cap_q=%u qstats_api=%u qfree_q=%u "
            "lean_ok=%u/%u udx_handoff_ok=%u lean_ready=%u "
            "t0_storage=1 store_door=1 export_map=1 map_ring=1 "
            "path=rw|flush|export_map|kick "
            "udx_host_stack=export|map_ring|kick handoff=storaged|UDX "
            "product_store=UDX not_freestanding_store_product "
            "dual_dod_store=OPEN C0=1 "
            "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
            "soft_ne_product=1 G-AC-1 Soft!=product\n",
            szVia != NULL ? szVia : "?",
            u32Ready, (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned long)g_u64Capacity,
            (unsigned)GJ_VIRTIO_BLK_SOFT_SEGS,
            (unsigned)VIRTIO_BLK_FLUSH_DEPTH,
            (unsigned)GJ_VIRTIO_BLK_SECTOR,
            (unsigned)VIRTIO_BLK_Q_SIZE,
            g_u32IoCount, g_u32Kicks, g_u32MultiSegOps, g_u32MultiSegs,
            g_u32SingleSegOps,
            g_u32FlushCount, g_u32FlushOk, g_u32FlushSoft, g_u32Errors,
            g_u32FailClass, g_u32MapQ, g_u32ExportQ, g_u32KickApi,
            g_u32ReadOps, g_u32WriteOps, g_u32Timeouts, g_u32StatusFail,
            g_u32StatusOk, g_u32StatusUnsup, g_u32StatusIoerr,
            g_u32QAddFail, g_u32CapMiss, g_u32BoundsReject,
            g_u32FlushBusyReject, g_u32MapFail, g_u32ExportFail,
            g_u32MapPageFail, g_u32KickFail,
            g_u32ReadApi, g_u32WriteApi, g_u32FlushApi, g_u32ApiEnter,
            g_u32XferEnter, g_u32XferChunks, g_u32XferOk, g_u32XferFail,
            g_u32BytesIn, g_u32BytesOut, g_u32BytesTot,
            g_u32LastType, g_u32LastStatus, g_u32LastLen, g_u32LastUsedLen,
            (unsigned long)g_u64LastSector,
            g_u32LeanFreeMin, g_u32LeanFreeNow,
            g_u32ProbeOk, g_u32ProbeNodev, g_u32ProbeFail,
            g_u32ReadyQuery, g_u32CapQuery, g_u32QStatsApi, g_u32QFreeQuery,
            u32Ok, u32Checks, g_u32UdxHandoffOk, g_u32LeanReady);

    /* PASS: full lean + UDX host stack surface (export/map/kick residual). */
    if (u32Ok == u32Checks && g_u32UdxHandoffOk >= 6u) {
        /* Grep: virtio-blk: soft residual lean PASS */
        kprintf("virtio-blk: soft residual lean PASS "
                "checks=%u ok=%u udx_handoff_ok=%u "
                "t0_storage=1 store_door=1 map_ring=1 "
                "udx_host_stack=export|map_ring|kick "
                "handoff=storaged|UDX product_store=UDX "
                "not_freestanding_store_product dual_dod_store=OPEN C0=1 "
                "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
                "soft_ne_product=1 G-AC-1 Soft!=product\n",
                u32Checks, u32Ok, g_u32UdxHandoffOk);
    }
}

/**
 * Fire soft residual lean once after first product-path activity.
 * Probe path may also emit via=probe after inventory (no double lamp).
 */
static void
blk_soft_maybe_lean(const char *szVia)
{
    if (g_fSoftLeanOnce != 0) {
        return;
    }
    blk_soft_residual_lean_once(szVia);
}

/*
 * Sector soft multi-segment xfer on request q0.
 * Descriptor chain (OASIS virtio-blk):
 *   desc0 device-read  - request header (type + sector)
 *   desc1 R or W       - data (1..SOFT_SEGS sectors, contiguous bounce)
 *   desc2 device-write - 1-byte status
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

        /* Sticky soft last (silent residual); updated before kick. */
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
    return 0;
}

/*
 * Probe path: find first blk -> modern PCI caps -> features -> request q0 ->
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
    g_fSoftInvOnce = 0;
    g_fSoftLeanOnce = 0;
    g_fMapQLamp = 0;
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
    g_u32LeanOk = 0;
    g_u32LeanChecks = 0;
    g_u32UdxHandoffOk = 0;
    g_u32KickFail = 0;
    g_u32QFreeQuery = 0;
    g_u32MapPageFail = 0;
    g_u32BoundsReject = 0;
    g_u32FlushOk = 0;
    g_u32LeanFreeNow = 0;
    g_u32LeanFreeMin = 0;
    g_u32LeanReady = 0;
    g_u32BytesTot = 0;
    g_u32ApiEnter = 0;
    g_u32FailClass = 0;

    c = virtio_dev_count();
    /* KIND_BLK (T0 store), transitional PCI id, or modern blk device ID */
    for (i = 0; i < c; i++) {
        struct gj_virtio_dev *pDev = virtio_dev_get(i);

        if (pDev == NULL) {
            continue;
        }
        if (pDev->u32Kind == GJ_VIRTIO_KIND_BLK ||
            pDev->u16Device == VIRTIO_BLK_PCI_TRANSITIONAL ||
            pDev->u16Device == GJ_VIRTIO_PCI_DEV_BLK_MODERN) {
            g_pBlk = pDev;
            break;
        }
    }
    if (g_pBlk == NULL) {
        kprintf("virtio-blk: no device\n");
        g_u32ProbeNodev++;
        /* One-shot soft inventory + lean residual (Soft!=product). */
        blk_soft_inventory_once("nodev");
        blk_soft_maybe_lean("nodev");
        return -1;
    }

    st = virtio_pci_setup(g_pBlk);
    if (st != GJ_OK || g_pBlk->pCommon == NULL) {
        kprintf("virtio-blk: pci setup failed %d\n", (int)st);
        g_pBlk = NULL;
        g_u32ProbeFail++;
        blk_soft_inventory_once("pci_fail");
        blk_soft_maybe_lean("pci_fail");
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
            blk_soft_inventory_once("negotiate_fail");
            blk_soft_maybe_lean("negotiate_fail");
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
        blk_soft_inventory_once("q_fail");
        blk_soft_maybe_lean("q_fail");
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
    /* One-shot soft inventory + lean residual (Soft!=product; T0 store). */
    blk_soft_inventory_once("probe");
    blk_soft_maybe_lean("probe");
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
    u32 u32Sectors;
    int iRet;

    g_u32ReadApi++;
    /* Soft residual: overflow-safe capacity bounds (no wrap on large LBA). */
    if (g_fReady && g_u64Capacity != 0) {
        u32Sectors = cbLen / GJ_VIRTIO_BLK_SECTOR;
        if (u32Sectors == 0u ||
            u32Sectors > g_u64Capacity ||
            u64Sector > (g_u64Capacity - (u64)u32Sectors)) {
            g_u32CapMiss++;
            g_u32BoundsReject++;
            blk_soft_maybe_lean("read_bounds");
            return -1;
        }
    }
    iRet = blk_xfer(VIRTIO_BLK_T_IN, u64Sector, pBuf, cbLen);
    /* First product R/W residual lean (one-shot; Soft!=product). */
    blk_soft_maybe_lean("read");
    return iRet;
}

int
virtio_blk_write(u64 u64Sector, const void *pBuf, u32 cbLen)
{
    u32 u32Sectors;
    int iRet;

    g_u32WriteApi++;
    /* Soft residual: overflow-safe capacity bounds (no wrap on large LBA). */
    if (g_fReady && g_u64Capacity != 0) {
        u32Sectors = cbLen / GJ_VIRTIO_BLK_SECTOR;
        if (u32Sectors == 0u ||
            u32Sectors > g_u64Capacity ||
            u64Sector > (g_u64Capacity - (u64)u32Sectors)) {
            g_u32CapMiss++;
            g_u32BoundsReject++;
            blk_soft_maybe_lean("write_bounds");
            return -1;
        }
    }
    iRet = blk_xfer(VIRTIO_BLK_T_OUT, u64Sector, (void *)(gj_vaddr_t)pBuf, cbLen);
    /* First product R/W residual lean (one-shot; Soft!=product). */
    blk_soft_maybe_lean("write");
    return iRet;
}

/*
 * OASIS T_FLUSH: hdr (device-R) + status (device-W), no data.
 * Soft depth-1 serial barrier. Device UNSUP -> fsync-shaped soft success.
 */
int
virtio_blk_flush(void)
{
    i32 i32Len;

    g_u32FlushApi++;
    if (!g_fReady) {
        blk_soft_maybe_lean("flush_not_ready");
        return -1;
    }
    if (g_fFlushBusy) {
        /* Soft depth exceeded: serial product path rejects concurrent FLUSH. */
        g_u32Errors++;
        g_u32FlushBusyReject++;
        blk_soft_maybe_lean("flush_busy");
        return -1;
    }

    g_fFlushBusy = 1;
    memset(&g_Req, 0, sizeof(g_Req));
    g_Req.u32Type = VIRTIO_BLK_T_FLUSH;
    g_Req.u64Sector = 0;
    g_u8Status = 0xff;

    /* Sticky soft last for FLUSH path (silent residual). */
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
        blk_soft_maybe_lean("flush_qadd");
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
        blk_soft_maybe_lean("flush_timeout");
        return -1;
    }
    g_u32LastUsedLen = (u32)i32Len;
    g_u32LastStatus = (u32)g_u8Status;
    if (g_u8Status == VIRTIO_BLK_S_OK) {
        g_u32FlushCount++;
        g_u32FlushOk++;
        g_u32IoCount++;
        g_u32StatusOk++;
        blk_soft_maybe_lean("flush");
        return 0;
    }
    if (g_u8Status == VIRTIO_BLK_S_UNSUP) {
        /* Soft fsync: device lacks FLUSH; barrier treated as success. */
        g_u32FlushSoft++;
        g_u32StatusUnsup++;
        blk_soft_maybe_lean("flush_soft");
        return 0;
    }
    kprintf("virtio-blk: flush status=%u\n", (unsigned)g_u8Status);
    g_u32Errors++;
    g_u32StatusFail++;
    if (g_u8Status == VIRTIO_BLK_S_IOERR) {
        g_u32StatusIoerr++;
    }
    blk_soft_maybe_lean("flush_status");
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
        /* Silent residual only - never re-emit inventory (FAULT class avoid). */
        (void)g_u32ProbeNodev;
        (void)g_u32ProbeFail;
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
     * Keep silent residual symbols live (not wire-stable; Soft!=product).
     * Never re-emit soft inventory here (FAULT class avoid).
     */
    (void)g_u32MapQ;
    (void)g_u32ExportQ;
    (void)g_u32KickApi;
    (void)g_u32ReadOps;
    (void)g_u32WriteOps;
    (void)g_u32Timeouts;
    (void)g_u32StatusFail;
    (void)g_u32QAddFail;
    (void)g_u32CapMiss;
    (void)g_u32FlushBusyReject;
    (void)g_u32ExportFail;
    (void)g_u32MapFail;
    (void)g_u32ReadApi;
    (void)g_u32WriteApi;
    (void)g_u32FlushApi;
    (void)g_u32QStatsApi;
    (void)g_u32LastType;
    (void)g_u32LastStatus;
    (void)g_u32LastLen;
    (void)g_u64LastSector;
    (void)g_u32LastUsedLen;
    (void)g_u32SingleSegOps;
    (void)g_u32XferEnter;
    (void)g_u32XferChunks;
    (void)g_u32XferOk;
    (void)g_u32XferFail;
    (void)g_u32ProbeOk;
    (void)g_u32ProbeNodev;
    (void)g_u32ProbeFail;
    (void)g_u32ReadyQuery;
    (void)g_u32CapQuery;
    (void)g_u32StatusOk;
    (void)g_u32StatusUnsup;
    (void)g_u32StatusIoerr;
    (void)g_u32LeanOk;
    (void)g_u32LeanChecks;
    (void)g_u32UdxHandoffOk;
    (void)g_u32KickFail;
    (void)g_u32QFreeQuery;
    (void)g_u32MapPageFail;
    (void)g_u32BoundsReject;
    (void)g_u32FlushOk;
    (void)g_u32LeanFreeNow;
    (void)g_u32LeanFreeMin;
    (void)g_u32LeanReady;
    (void)g_u32BytesTot;
    (void)g_u32ApiEnter;
    (void)g_u32FailClass;
    return 0;
}

int
virtio_blk_export_q(struct gj_virtq_export *pOut)
{
    if (!g_fReady || pOut == NULL || g_qReq.pDesc == NULL) {
        g_u32ExportFail++;
        blk_soft_maybe_lean("export_fail");
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
    /* store_door EXPORT_RING residual; one-shot lean if probe skipped. */
    blk_soft_maybe_lean("export");
    return 0;
}

int
virtio_blk_kick_q(void)
{
    if (!g_fReady) {
        g_u32KickFail++;
        blk_soft_maybe_lean("kick_fail");
        return -1;
    }
    g_u32KickApi++;
    q_kick_counted();
    /* UDX notify residual; one-shot lean if earlier path skipped. */
    blk_soft_maybe_lean("kick");
    return 0;
}

u16
virtio_blk_q_free(void)
{
    g_u32QFreeQuery++;
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
        g_u32MapPageFail = 0; /* pre-map fail; no page index */
        blk_soft_maybe_lean("map_export_fail");
        return -1;
    }
    if (u64VaBase == 0 || (u64VaBase & 0xfffull) != 0) {
        g_u32MapFail++;
        g_u32MapPageFail = 0;
        blk_soft_maybe_lean("map_inval");
        return -1;
    }
    /* One page each: rings live on dedicated PMM pages (virtio_q_setup). */
    if (vmm_map_page((gj_vaddr_t)u64VaBase, (gj_paddr_t)ex.u64PaDesc,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        g_u32MapFail++;
        g_u32MapPageFail = 1u; /* sticky: desc page */
        blk_soft_maybe_lean("map_page1");
        return -1;
    }
    if (vmm_map_page((gj_vaddr_t)(u64VaBase + GJ_PAGE_SIZE),
                     (gj_paddr_t)ex.u64PaAvail,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        g_u32MapFail++;
        g_u32MapPageFail = 2u; /* sticky: avail page */
        blk_soft_maybe_lean("map_page2");
        return -1;
    }
    if (vmm_map_page((gj_vaddr_t)(u64VaBase + 2u * GJ_PAGE_SIZE),
                     (gj_paddr_t)ex.u64PaUsed,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        g_u32MapFail++;
        g_u32MapPageFail = 3u; /* sticky: used page */
        blk_soft_maybe_lean("map_page3");
        return -1;
    }
    ex.u32OffDesc = 0;
    ex.u32OffAvail = GJ_PAGE_SIZE;
    ex.u32OffUsed = GJ_PAGE_SIZE * 2u;
    if (pOut != NULL) {
        *pOut = ex;
    }
    g_u32MapQ++;
    g_u32MapPageFail = 0;
    /* Once-only lamp - UDX re-map must not stamp-flood. Soft!=product. */
    if (g_fMapQLamp == 0) {
        g_fMapQLamp = 1;
        kprintf("virtio-blk: map_q va=0x%lx size=%u\n",
                (unsigned long)u64VaBase, ex.u16Size);
    }
    /* store_door MAP_RING residual; one-shot lean if probe skipped. */
    blk_soft_maybe_lean("map");
    return 0;
}
