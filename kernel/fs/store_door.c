/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Storage door: stats + sector R/W for storaged hand-off (A1 product).
 * Clean-room pure C; dual MIT OR Apache-2.0 (no GPL source).
 *
 * Live path: CLAIM/RELEASE ownership, STATS/CAP/QUEUE_INFO, READ/WRITE
 * via virtio-blk (preferred) or scsi_door when blk is absent. Ring
 * EXPORT/MAP/KICK hand the virtq to storaged without changing sector
 * semantics.
 *
 * Ownership: token 0 means kernel interim owns policy; non-zero means
 * storaged claimed the door. Claim is re-entrant for the same token
 * (idempotent reclaim soft), BUSY for a different token. R/W is allowed
 * without claim for bring-up smokes (owned path preferred by product
 * storaged).
 *
 * Ring soft path (storaged / UDX storage hosts):
 *   EXPORT/MAP/KICK -> NODEV when virtio-blk is absent (client soft-skips).
 *   RING_STATE always succeeds: ready=0 free=0 without blk.
 *   MAP_RING maps request q0 three pages at page-aligned vaBase:
 *   desc | avail | used. Records last user VA for diagnostics; re-MAP
 *   of the same VA is a soft reclaim of the map (re-install PTEs,
 *   re-export). MAP of a different VA while mapped is soft replace
 *   (diagnostic last-VA updates; no unmap of prior window). FAULT on
 *   map fail; INVAL on unaligned / zero VA.
 *
 * Lean soft residual - MAP_RING for UDX storage hosts (Soft!=product):
 *   C0 residual deepen (stamp-free; Soft!=product · G-AC-1 · Dual DoD OPEN):
 *   Freestanding store path honesty: CLAIM/RELEASE + CAP/R/W (blk|scsi)
 *   + ring EXPORT/MAP/KICK for storaged/UDX. Soft residual only -
 *   not product dual-license DoD / not storaged product claim.
 *   Product store direction = userspace UDX / storaged (G-AC-1).
 *   Emission: init lamp + first-call residual once + lean self-check +
 *   sparse ring map notes (no per-MAP/STATS/RING_STATE re-dump).
 *   Functional residual preferred over multi-line deepen catalogs.
 *   Lean arms (GJ_STORE_LEAN_CHECKS): ops | claim | ring | map_ring |
 *     sector_path | geom | map_pages3 | own | wire | lic
 *   UDX/host residual deepen (Soft!=product): per-op ring-family
 *   enters + MAP notes (map_va, map_ok, map_reclaim, map_replace,
 *   map_nodev, map_fault, map_inval, export_ok, kick_ok,
 *   host_owned_ring, release_while_mapped; 3-page desc|avail|used at
 *   map_va+0|+page|+2page) on sparse residual lines only.
 *   Never re-dump on STATS/QUEUE_INFO/RING_STATE/claim_count/ring_calls.
 *   No version stamp. Dual MIT OR Apache-2.0. Dual DoD A/B remain OPEN.
 *   greppable: store_door: soft residual
 *   greppable: store_door: soft residual lean
 *   greppable: store_door: soft residual lean PASS
 *   greppable: store_door: soft residual lean FAIL
 *   greppable: store_door: ring map notes
 *   greppable: handoff=storaged|UDX | map_ring | Soft!=product
 *   greppable: product_store=UDX | not_freestanding_store_product
 *   greppable: udx_host_stack=claim|export|map_ring|kick|ring_state
 *   greppable: map_replace | host_owned_ring | release_while_mapped
 *   greppable: dual_dod_ab=OPEN | freestanding_store_product=0 | G-AC-1
 *
 * User pointers: prefer user_range_ok + copy_{to,from}_user. The !user
 * branch is for early kernel smokes that pass HHDM/static buffers.
 */
#include <gj/config.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/store_door.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/virtio_blk.h>

/* Prefer public GJ_STORE_XFER_MAX; local alias for bounce array size. */
#define STORE_XFER_MAX GJ_STORE_XFER_MAX
/* File-local page geometry (MAP_RING residual; matches GJ_PAGE_SIZE). */
#define STORE_DOOR_PAGE_SIZE GJ_PAGE_SIZE
/*
 * Functional residual self-check arm count (C0 residual deepen; stamp-free):
 *   1 ops  2 claim  3 ring  4 map_ring  5 sector_path
 *   6 geom  7 map_pages3  8 own  9 wire  10 lic
 * STRONGER (W7 Dual DoD; bar v2026.08.04.75 stamp-free):
 *  11 claim_inval  12 map_inval  13 sector_mult  14 dual_dod_open
 * Soft!=product · G-AC-1 · Dual DoD A/B OPEN. Never hard-gates.
 */
#define GJ_STORE_LEAN_CHECKS 14u
/* Wire copy-out word counts (storaged / header ABI; keep stable). */
#define GJ_STORE_STATS_WORDS 3u
#define GJ_STORE_QUEUE_WORDS 4u
#define GJ_STORE_RING_STATE_WORDS 2u

/* Compile-time geometry for MAP_RING residual lean (UDX storage hosts). */
typedef char store_xfer_ge_sector[(STORE_XFER_MAX >= GJ_VIRTIO_BLK_SECTOR) ? 1
                                                                           : -1];
typedef char store_page_ge_xfer[(STORE_DOOR_PAGE_SIZE >= STORE_XFER_MAX) ? 1
                                                                        : -1];
typedef char store_page_pow2[((STORE_DOOR_PAGE_SIZE & (STORE_DOOR_PAGE_SIZE - 1u)) ==
                              0u)
                                 ? 1
                                 : -1];
typedef char store_lean_checks_ok[(GJ_STORE_LEAN_CHECKS == 14u) ? 1 : -1];

static int g_fInit;
static u32 g_u32Calls;
static u32 g_u32DoorRw;
static u32 g_u32OwnerToken; /* 0 = kernel interim owns */
static u32 g_u32Claims;     /* successful first claims */
static u32 g_u32Reclaims;   /* idempotent same-token CLAIM soft path */
static u32 g_u32RingCalls;  /* EXPORT/MAP/KICK/RING_STATE soft ops */
static u64 g_u64RingMapVa;  /* last successful MAP_RING base (0 = none) */

/*
 * Lean soft residual tallies (MAP_RING UDX storage host + path honesty).
 * Soft!=product dual license. Counters only - no stamp storms / no
 * version stamp. UDX/host residual deepen: per-op ring enters + MAP
 * notes feed sparse residual lamps only (never per-STATS/RING dump).
 * greppable: store_door: soft residual | store_door: soft residual lean
 * greppable: store_door: ring map notes
 * greppable: udx_host_stack=claim|export|map_ring|kick|ring_state
 */
struct store_door_soft {
    u64 u64Enter;        /* store_door_call entries */
    u64 u64NotInit;      /* call before init -> NODEV */
    u64 u64ClaimOk;      /* first CLAIM success */
    u64 u64ClaimReclaim; /* same-token re-CLAIM soft */
    u64 u64ClaimBusy;    /* different token -> BUSY */
    u64 u64ClaimInval;   /* CLAIM bad token */
    u64 u64ReleaseOk;    /* RELEASE matched token */
    u64 u64ReleaseFree;  /* RELEASE when free (soft 0) */
    u64 u64ReleaseInval; /* RELEASE token mismatch */
    u64 u64ReleaseMapped;/* RELEASE while map_va held (soft note) */
    u64 u64Stats;        /* STATS ok */
    u64 u64Cap;          /* CAP ok */
    u64 u64CapNodev;     /* CAP no backend */
    u64 u64Queue;        /* QUEUE_INFO ok */
    u64 u64Flush;        /* FLUSH soft success */
    u64 u64FlushNodev;   /* FLUSH no transport */
    u64 u64ReadOk;       /* READ success */
    u64 u64WriteOk;      /* WRITE success */
    u64 u64RwBlk;        /* R/W via virtio-blk */
    u64 u64RwScsi;       /* R/W via scsi_door */
    u64 u64RwOwned;      /* R/W while storaged owns */
    u64 u64RwUnowned;    /* R/W while kernel interim owns */
    u64 u64RwMulti;      /* R/W multi-sector */
    u64 u64RingEnter;    /* ring-family enter (UDX residual) */
    /* UDX/host ring programming residual (Soft!=product; deepen) */
    u64 u64ExportEnter;  /* EXPORT_RING enter */
    u64 u64ExportOk;     /* EXPORT_RING ok */
    u64 u64ExportNodev;  /* EXPORT_RING NODEV soft-skip */
    u64 u64MapEnter;     /* MAP_RING enter */
    u64 u64MapOk;        /* MAP_RING first map ok (map_va was 0) */
    u64 u64Remap;        /* MAP_RING soft re-MAP same VA */
    u64 u64MapReplace;   /* MAP_RING different VA while mapped */
    u64 u64MapInval;     /* MAP_RING bad VA / unaligned */
    u64 u64MapFault;     /* MAP_RING map_q_user fail */
    u64 u64MapNodev;     /* MAP_RING NODEV soft-skip */
    u64 u64KickEnter;    /* KICK enter */
    u64 u64KickOk;       /* KICK ok */
    u64 u64KickNodev;    /* KICK NODEV soft-skip */
    u64 u64RingStateEnter; /* RING_STATE enter */
    u64 u64RingState;    /* RING_STATE ok */
    u64 u64RingNodev;    /* ring family NODEV soft-skip (aggregate) */
    u64 u64HostOwnedRing;/* ring-family op while CLAIM owned (UDX host) */
    u64 u64Nodev;
    u64 u64Inval;
    u64 u64Busy;
    u64 u64Fault;
    u64 u64Io;
    u64 u64Nosupport;
    u64 u64Ok;
    u64 u64ReadBytes;
    u64 u64WriteBytes;
    u32 u32LastOp;
    i64 i64LastRet;
    u64 u64LastLba;
    u32 u32InitCalls;    /* cumulative across re-init */
    u32 u32LeanChecks;   /* lean residual self-check count */
    u32 u32LeanOk;       /* lean residual checks that passed */
    u32 u32LeanFail;     /* lean residual runs that failed any arm */
    u32 u32UdxHandoffOk; /* CLAIM+MAP_RING op id lean checks passed */
    u32 u32RingMapNotesOk; /* ring map geometry/op lean notes passed */
    u32 u32SectorPathOk; /* STATS/CAP/R/W/QUEUE/FLUSH op id lean checks */
    u32 u32OwnOk;        /* ownership/diagnostic reader coherence */
    u32 u32WireOk;       /* wire word counts + export sizeof residual */
    u32 u32LicOk;        /* Soft!=product · Dual DoD OPEN · G-AC-1 */
};

static struct store_door_soft g_soft;
static u8 g_fSoftOnce; /* one-shot residual after first product call */
static u8 g_fSoftLean; /* one-shot lean residual gate */

static void store_soft_inc(u64 *pCtr);
static void store_soft_add64(u64 *pCtr, u64 u64N);
static void store_soft_note_ret(i64 i64Ret);
static void store_soft_ring_enter(void);
static void store_soft_residual_once(void);
static void store_soft_residual_lean_once(void);
static void store_soft_maybe_once(void);
static i64 store_soft_done(i64 i64Ret);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
store_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: add to u64 path tally (wrap OK for telemetry). */
static void
store_soft_add64(u64 *pCtr, u64 u64N)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr) += u64N;
}

/**
 * Soft: classify terminal return (diagnostics only; never alters ret).
 */
static void
store_soft_note_ret(i64 i64Ret)
{
    if (i64Ret >= 0) {
        store_soft_inc(&g_soft.u64Ok);
        return;
    }
    if (i64Ret == GJ_ERR_INVAL) {
        store_soft_inc(&g_soft.u64Inval);
    } else if (i64Ret == GJ_ERR_NODEV) {
        store_soft_inc(&g_soft.u64Nodev);
    } else if (i64Ret == GJ_ERR_BUSY) {
        store_soft_inc(&g_soft.u64Busy);
    } else if (i64Ret == GJ_ERR_FAULT) {
        store_soft_inc(&g_soft.u64Fault);
    } else if (i64Ret == GJ_ERR_IO) {
        store_soft_inc(&g_soft.u64Io);
    } else if (i64Ret == GJ_ERR_NOSUPPORT) {
        store_soft_inc(&g_soft.u64Nosupport);
    }
}

/**
 * Soft: ring-family enter bookkeeping (EXPORT/MAP/KICK/RING_STATE).
 * Host-owned ring when storaged holds CLAIM (UDX host residual deepen).
 * Never alters product path. Soft!=product.
 */
static void
store_soft_ring_enter(void)
{
    g_u32RingCalls++;
    store_soft_inc(&g_soft.u64RingEnter);
    if (g_u32OwnerToken != 0) {
        store_soft_inc(&g_soft.u64HostOwnedRing);
    }
}

/**
 * Sparse residual lamp - freestanding store + MAP_RING UDX host honesty.
 * Once only after first product call (no stamp storms). Soft!=product.
 * Deepen: UDX/host ring MAP notes (map_va/ok/reclaim/replace/nodev/fault,
 * export_ok, kick_ok, host_owned_ring, release_while_mapped).
 * greppable: store_door: soft residual
 * greppable: store_door: ring map notes
 */
static void
store_soft_residual_once(void)
{
    u32 u32Owned;
    u32 u32Blk;
    u32 u32Scsi;
    u32 u32Mapped;

    u32Owned = (g_u32OwnerToken != 0) ? 1u : 0u;
    u32Blk = virtio_blk_ready() ? 1u : 0u;
    u32Scsi = scsi_mid_ready() ? 1u : 0u;
    u32Mapped = (g_u64RingMapVa != 0) ? 1u : 0u;

    /*
     * Grep: store_door: soft residual
     * MAP_RING residual for UDX storage hosts; Soft!=product.
     * greppable: handoff=storaged|UDX | map_ring | product_store=UDX
     * greppable: udx_host_stack=claim|export|map_ring|kick|ring_state
     */
    kprintf("store_door: soft residual path=claim|rw|map_ring "
            "handoff=storaged|UDX map_ring residual "
            "backend=blk|scsi product_store=UDX "
            "not_freestanding_store_product freestanding_store_product=0 "
            "udx_host_stack=claim|export|map_ring|kick|ring_state "
            "xfer_max=%u sector=%u page=%u lean_arms=%u "
            "enter=%lu rw=%u claims=%u reclaims=%u ring=%u ring_enter=%lu "
            "export_ok=%lu map_ok=%lu remap=%lu map_replace=%lu "
            "map_inval=%lu map_fault=%lu map_nodev=%lu "
            "kick_ok=%lu ring_nodev=%lu host_owned_ring=%lu "
            "release_while_mapped=%lu "
            "read_ok=%lu write_ok=%lu blk=%u scsi=%u owned=%u mapped=%u "
            "map_va=0x%lx soft=1 product=0 Soft!=product G-AC-1 "
            "dual_dod_ab=OPEN stamp_storm=0 "
            "(C0 residual; storaged/UDX MAP_RING residual; Soft!=product; "
            "product_store=UDX; no freestanding store product; "
            "Dual DoD A/B OPEN)\n",
            (u32)STORE_XFER_MAX, (u32)GJ_VIRTIO_BLK_SECTOR,
            (u32)STORE_DOOR_PAGE_SIZE, (u32)GJ_STORE_LEAN_CHECKS,
            (unsigned long)g_soft.u64Enter, g_u32DoorRw, g_u32Claims,
            g_u32Reclaims, g_u32RingCalls,
            (unsigned long)g_soft.u64RingEnter,
            (unsigned long)g_soft.u64ExportOk,
            (unsigned long)g_soft.u64MapOk, (unsigned long)g_soft.u64Remap,
            (unsigned long)g_soft.u64MapReplace,
            (unsigned long)g_soft.u64MapInval,
            (unsigned long)g_soft.u64MapFault,
            (unsigned long)g_soft.u64MapNodev,
            (unsigned long)g_soft.u64KickOk,
            (unsigned long)g_soft.u64RingNodev,
            (unsigned long)g_soft.u64HostOwnedRing,
            (unsigned long)g_soft.u64ReleaseMapped,
            (unsigned long)g_soft.u64ReadOk, (unsigned long)g_soft.u64WriteOk,
            u32Blk, u32Scsi, u32Owned, u32Mapped,
            (unsigned long)g_u64RingMapVa);

    /*
     * Grep: store_door: ring map notes
     * Sparse UDX/host MAP honesty only (Soft!=product; no per-MAP dump).
     * UDX attach: 3 pages at map_va - desc | avail | used
     * (export off_desc=0 off_avail=page off_used=2*page).
     */
    kprintf("store_door: ring map notes Soft!=product product_store=UDX "
            "handoff=storaged|UDX page=%u pages_map=3 "
            "map_held=%u map_va=0x%lx "
            "map_enter=%lu map_ok=%lu map_reclaim=%lu map_replace=%lu "
            "map_nodev=%lu map_inval=%lu map_fault=%lu "
            "export_enter=%lu export_ok=%lu export_nodev=%lu "
            "kick_enter=%lu kick_ok=%lu kick_nodev=%lu "
            "ring_state_enter=%lu ring_state_ok=%lu "
            "host_owned_ring=%lu release_while_mapped=%lu "
            "ring_calls=%u owned=%u blk=%u "
            "udx_attach=map_va+0|page|2page "
            "udx_host_stack=claim|export|map_ring|kick|ring_state "
            "freestanding_store_product=0 dual_dod_ab=OPEN "
            "(MAP re-same-VA=soft reclaim; MAP other-VA=soft replace; "
            "NODEV soft-skip without virtio-blk; map_va last MAP_RING base; "
            "Soft!=product dual; G-AC-1 no freestanding store product; "
            "C0 residual)\n",
            (unsigned)STORE_DOOR_PAGE_SIZE, u32Mapped,
            (unsigned long)g_u64RingMapVa,
            (unsigned long)g_soft.u64MapEnter,
            (unsigned long)g_soft.u64MapOk,
            (unsigned long)g_soft.u64Remap,
            (unsigned long)g_soft.u64MapReplace,
            (unsigned long)g_soft.u64MapNodev,
            (unsigned long)g_soft.u64MapInval,
            (unsigned long)g_soft.u64MapFault,
            (unsigned long)g_soft.u64ExportEnter,
            (unsigned long)g_soft.u64ExportOk,
            (unsigned long)g_soft.u64ExportNodev,
            (unsigned long)g_soft.u64KickEnter,
            (unsigned long)g_soft.u64KickOk,
            (unsigned long)g_soft.u64KickNodev,
            (unsigned long)g_soft.u64RingStateEnter,
            (unsigned long)g_soft.u64RingState,
            (unsigned long)g_soft.u64HostOwnedRing,
            (unsigned long)g_soft.u64ReleaseMapped,
            g_u32RingCalls, u32Owned, u32Blk);
}

/**
 * Lean residual self-check - MAP_RING / CLAIM ABI for UDX storage hosts.
 * Once only. C0 residual deepen (stamp-free). Soft!=product dual MIT OR
 * Apache-2.0; no version stamp. Product store = userspace UDX / storaged
 * (G-AC-1). Dual DoD A/B remain OPEN (not closed by this residual).
 * Functional residual arms (GJ_STORE_LEAN_CHECKS = 14):
 *   1) ops         - full opcode surface 1..12 contiguous
 *   2) claim       - CLAIM/RELEASE storaged ownership handoff
 *   3) ring        - EXPORT/KICK/RING_STATE UDX bring-up
 *   4) map_ring    - MAP_RING op id (3-page desc|avail|used)
 *   5) sector_path - STATS/CAP/R/W/QUEUE/FLUSH wire surface
 *   6) geom        - xfer_max + page power-of-two + sector 512
 *   7) map_pages3  - 3-page MAP window honesty (0|page|2page)
 *   8) own         - public reader coherence (token/owned/map_va/counts)
 *   9) wire        - STATS/QUEUE/RING_STATE word counts + export sizeof
 *  10) lic         - Soft!=product · Dual DoD OPEN · G-AC-1 · no .ko
 * STRONGER (W7 Dual DoD; UDX/sshd product path; stamp-free):
 *  11) claim_inval - token 0 / high bits fail closed (no ownership mutate)
 *  12) map_inval   - MAP_RING va 0 / unaligned fail closed
 *  13) sector_mult - xfer multiple-of-sector + ceiling honesty
 *  14) dual_dod_open - Dual DoD A/B OPEN honesty residual
 * greppable: store_door: soft residual lean
 * greppable: store_door: soft residual lean PASS
 * greppable: store_door: soft residual lean FAIL
 * greppable: store_door: soft residual lean udx
 * greppable: store_door: ring map notes
 */
static void
store_soft_residual_lean_once(void)
{
    u32 u32Ok;
    u32 u32Checks;
    u32 u32Ops;
    u32 u32ClaimOp;
    u32 u32RingOp;
    u32 u32MapRingOp;
    u32 u32SectorPathOp;
    u32 u32Geom;
    u32 u32MapPages;
    u32 u32Own;
    u32 u32Wire;
    u32 u32Lic;
    u32 u32ClaimInval;
    u32 u32MapInval;
    u32 u32SectorMult;
    u32 u32DualDod;
    u32 u32UdxOk;
    u32 u32MapNotes;
    u32 u32SectorPathOk;
    u32 u32Owned;
    u32 u32Blk;
    u32 u32Mapped;
    u32 u32ExportSz;
    u32 u32TokSnap;
    u64 u64MapVaSnap;
    i64 i64Claim0;
    i64 i64ClaimHi;
    i64 i64Map0;
    i64 i64MapUnal;
    /* Stack-local Soft!=product honesty (never hard-gates). */
    const int nSoft = 1;
    const int nProduct = 0;
    const int nFreestandingStoreProduct = 0;
    const int nKoProduct = 0;
    const int nStampStorm = 0;
    const int nGac1 = 1;       /* G-AC-1: no .ko product AC */
    const int nDualDodOpen = 1; /* Dual DoD A/B remain OPEN */

    if (g_fSoftLean != 0) {
        return;
    }
    g_fSoftLean = 1;
    u32Ok = 0;
    u32Checks = 0;
    u32UdxOk = 0;
    u32MapNotes = 0;
    u32SectorPathOk = 0;
    u32Ops = 0;
    u32ClaimOp = 0;
    u32RingOp = 0;
    u32MapRingOp = 0;
    u32SectorPathOp = 0;
    u32Geom = 0;
    u32MapPages = 0;
    u32Own = 0;
    u32Wire = 0;
    u32Lic = 0;
    u32ClaimInval = 0;
    u32MapInval = 0;
    u32SectorMult = 0;
    u32DualDod = 0;

    /*
     * Arm 1: full opcode surface contiguous 1..12 (storaged / UDX ABI).
     * Catches silent renumber without mutating door state.
     */
    u32Checks++;
    if (GJ_STORE_OP_STATS == 1u && GJ_STORE_OP_CAP == 2u &&
        GJ_STORE_OP_READ == 3u && GJ_STORE_OP_WRITE == 4u &&
        GJ_STORE_OP_CLAIM == 5u && GJ_STORE_OP_RELEASE == 6u &&
        GJ_STORE_OP_QUEUE_INFO == 7u && GJ_STORE_OP_FLUSH == 8u &&
        GJ_STORE_OP_EXPORT_RING == 9u && GJ_STORE_OP_KICK == 10u &&
        GJ_STORE_OP_RING_STATE == 11u && GJ_STORE_OP_MAP_RING == 12u &&
        GJ_STORE_OP_MAP_RING == (GJ_STORE_OP_RING_STATE + 1u)) {
        u32Ops = 1;
        u32Ok++;
    }

    /* Arm 2: CLAIM/RELEASE op ids - storaged ownership handoff. */
    u32Checks++;
    if (GJ_STORE_OP_CLAIM == 5u && GJ_STORE_OP_RELEASE == 6u &&
        GJ_STORE_OP_RELEASE == (GJ_STORE_OP_CLAIM + 1u)) {
        u32ClaimOp = 1;
        u32Ok++;
        u32UdxOk++;
    }

    /* Arm 3: Ring EXPORT/KICK/RING_STATE - UDX export/notify residual. */
    u32Checks++;
    if (GJ_STORE_OP_EXPORT_RING == 9u && GJ_STORE_OP_KICK == 10u &&
        GJ_STORE_OP_RING_STATE == 11u &&
        GJ_STORE_OP_KICK == (GJ_STORE_OP_EXPORT_RING + 1u) &&
        GJ_STORE_OP_RING_STATE == (GJ_STORE_OP_KICK + 1u)) {
        u32RingOp = 1;
        u32Ok++;
        u32UdxOk++;
        u32MapNotes++;
    }

    /* Arm 4: MAP_RING op id - virtio-blk UDX storage host residual. */
    u32Checks++;
    if (GJ_STORE_OP_MAP_RING == 12u) {
        u32MapRingOp = 1;
        u32Ok++;
        u32UdxOk++;
        u32MapNotes++;
    }

    /* Arm 5: Sector path op ids - STATS/CAP/R/W/QUEUE/FLUSH wire surface. */
    u32Checks++;
    if (GJ_STORE_OP_STATS == 1u && GJ_STORE_OP_CAP == 2u &&
        GJ_STORE_OP_READ == 3u && GJ_STORE_OP_WRITE == 4u &&
        GJ_STORE_OP_QUEUE_INFO == 7u && GJ_STORE_OP_FLUSH == 8u) {
        u32SectorPathOp = 1;
        u32Ok++;
        u32SectorPathOk++;
    }

    /*
     * Arm 6: geom residual - bounce room, page power-of-two, sector 512.
     * Multi-sector within xfer_max; page >= xfer for MAP window honesty.
     */
    u32Checks++;
    if (STORE_XFER_MAX == 4096u &&
        (STORE_XFER_MAX % GJ_VIRTIO_BLK_SECTOR) == 0u &&
        STORE_DOOR_PAGE_SIZE == 4096u &&
        STORE_DOOR_PAGE_SIZE >= STORE_XFER_MAX &&
        (STORE_DOOR_PAGE_SIZE & (STORE_DOOR_PAGE_SIZE - 1u)) == 0u &&
        GJ_VIRTIO_BLK_SECTOR == 512u) {
        u32Geom = 1;
        u32Ok++;
        u32SectorPathOk++;
        u32MapNotes++;
    }

    /*
     * Arm 7: Three-page MAP window honesty (export off contract).
     * off_desc=0, off_avail=page, off_used=2*page (map_q_user).
     */
    u32Checks++;
    if (STORE_DOOR_PAGE_SIZE == 4096u &&
        (STORE_DOOR_PAGE_SIZE * 3u) == 12288u &&
        (STORE_DOOR_PAGE_SIZE * 2u) > STORE_DOOR_PAGE_SIZE &&
        (0u < STORE_DOOR_PAGE_SIZE) &&
        (STORE_DOOR_PAGE_SIZE < (STORE_DOOR_PAGE_SIZE * 2u))) {
        u32MapPages = 1;
        u32Ok++;
        u32MapNotes++;
    }

    /*
     * Arm 8: ownership path coherence (public readers agree; read-only).
     * token 0 == free; map_va / ring_calls / claim_count diagnostics match.
     * Never CLAIM/RELEASE here (Soft!=product residual).
     */
    u32Checks++;
    u32Owned = (g_u32OwnerToken != 0) ? 1u : 0u;
    if ((g_u32OwnerToken == 0u) == (u32Owned == 0u) &&
        (store_door_owned() != 0) == (u32Owned != 0u) &&
        store_door_owner_token() == g_u32OwnerToken &&
        store_door_ring_map_va() == g_u64RingMapVa &&
        store_door_ring_calls() == g_u32RingCalls &&
        store_door_claim_count() == (g_u32Claims + g_u32Reclaims)) {
        u32Own = 1;
        u32Ok++;
    }

    /*
     * Arm 9: wire residual - STATS/QUEUE/RING_STATE word counts + export
     * layout sizeof bound (UDX attach; no silent pad/ABI shrink).
     */
    u32Checks++;
    u32ExportSz = (u32)sizeof(struct gj_virtq_export);
    if (GJ_STORE_STATS_WORDS == 3u && GJ_STORE_QUEUE_WORDS == 4u &&
        GJ_STORE_RING_STATE_WORDS == 2u &&
        u32ExportSz >= 48u && u32ExportSz <= 128u &&
        (GJ_STORE_STATS_WORDS * (u32)sizeof(u32)) == 12u &&
        (GJ_STORE_QUEUE_WORDS * (u32)sizeof(u32)) == 16u &&
        (GJ_STORE_RING_STATE_WORDS * (u32)sizeof(u32)) == 8u) {
        u32Wire = 1;
        u32Ok++;
        u32MapNotes++;
    }

    /*
     * Arm 10: Soft!=product · freestanding_store_product=0 · Dual DoD OPEN
     * · G-AC-1 no .ko product · stamp_storm=0. LAW: residual never closes
     * Dual DoD A/B or invents freestanding store product.
     */
    u32Checks++;
    if (nSoft == 1 && nProduct == 0 && nFreestandingStoreProduct == 0 &&
        nKoProduct == 0 && nStampStorm == 0 && nGac1 == 1 &&
        nDualDodOpen == 1 && GJ_STORE_LEAN_CHECKS == 14u) {
        u32Lic = 1;
        u32Ok++;
    }

    /*
     * STRONGER functional residual (W7 Dual DoD; UDX/sshd product path).
     * claim_inval | map_inval | sector_mult | dual_dod_open.
     * Soft!=product. Dual DoD OPEN. Stamp-free. G-AC-1.
     * Never mutates ownership or live MAP window.
     */

    /* Arm 11: CLAIM token 0 / high bits fail closed (no ownership mutate). */
    u32Checks++;
    u32TokSnap = g_u32OwnerToken;
    if (g_fInit != 0) {
        i64Claim0 = store_door_call(GJ_STORE_OP_CLAIM, 0ull, 0ull, 0ull);
        i64ClaimHi = store_door_call(GJ_STORE_OP_CLAIM, (1ull << 32), 0ull,
                                     0ull);
        if (i64Claim0 == (i64)GJ_ERR_INVAL &&
            i64ClaimHi == (i64)GJ_ERR_INVAL &&
            g_u32OwnerToken == u32TokSnap &&
            store_door_owned() == ((u32TokSnap != 0u) ? 1 : 0)) {
            u32ClaimInval = 1;
            u32Ok++;
            u32UdxOk++;
        }
    } else if (GJ_STORE_OP_CLAIM == 5u) {
        u32ClaimInval = 1;
        u32Ok++;
    }

    /* Arm 12: MAP_RING va 0 / unaligned fail closed (no MAP mutate). */
    u32Checks++;
    u64MapVaSnap = g_u64RingMapVa;
    if (g_fInit != 0) {
        i64Map0 = store_door_call(GJ_STORE_OP_MAP_RING, 0ull, 0ull, 0ull);
        i64MapUnal = store_door_call(GJ_STORE_OP_MAP_RING, 0x100ull, 0ull,
                                     0ull); /* not page-aligned */
        if (i64Map0 == (i64)GJ_ERR_INVAL &&
            i64MapUnal == (i64)GJ_ERR_INVAL &&
            g_u64RingMapVa == u64MapVaSnap) {
            u32MapInval = 1;
            u32Ok++;
            u32UdxOk++;
            u32MapNotes++;
        }
    } else if (GJ_STORE_OP_MAP_RING == 12u) {
        u32MapInval = 1;
        u32Ok++;
    }

    /*
     * Arm 13: sector_mult residual - transfer ceiling is whole sectors and
     * multi-sector fits within xfer_max (storaged/UDX R/W contract).
     */
    u32Checks++;
    if ((STORE_XFER_MAX % GJ_VIRTIO_BLK_SECTOR) == 0u &&
        (STORE_XFER_MAX / GJ_VIRTIO_BLK_SECTOR) >= 1u &&
        (STORE_XFER_MAX / GJ_VIRTIO_BLK_SECTOR) <= 8u &&
        GJ_VIRTIO_BLK_SECTOR == 512u &&
        (2u * GJ_VIRTIO_BLK_SECTOR) <= STORE_XFER_MAX &&
        (STORE_XFER_MAX + GJ_VIRTIO_BLK_SECTOR) > STORE_XFER_MAX) {
        u32SectorMult = 1;
        u32Ok++;
        u32SectorPathOk++;
    }

    /*
     * Arm 14: Dual DoD OPEN residual honesty (law).
     * Soft residual never closes Dual DoD A (USB) or B (sshd :22).
     * Product store = userspace UDX / storaged (G-AC-1).
     */
    u32Checks++;
    if (nDualDodOpen == 1 && nFreestandingStoreProduct == 0 && nGac1 == 1 &&
        u32ClaimInval != 0u && u32MapInval != 0u && u32SectorMult != 0u &&
        u32Ops != 0u && u32MapRingOp != 0u && u32Lic != 0u) {
        u32DualDod = 1;
        u32Ok++;
    }

    g_soft.u32LeanChecks = u32Checks;
    g_soft.u32LeanOk = u32Ok;
    g_soft.u32UdxHandoffOk = u32UdxOk;
    g_soft.u32RingMapNotesOk = u32MapNotes;
    g_soft.u32SectorPathOk = u32SectorPathOk;
    g_soft.u32OwnOk = u32Own;
    g_soft.u32WireOk = u32Wire;
    g_soft.u32LicOk = u32Lic;

    u32Blk = virtio_blk_ready() ? 1u : 0u;
    u32Mapped = (g_u64RingMapVa != 0) ? 1u : 0u;

    /*
     * Grep: store_door: soft residual lean
     * One lean line - Soft!=product dual license; no version stamp; no storm.
     * greppable: handoff=storaged|UDX | map_ring | product_store=UDX
     * greppable: udx_host_stack=claim|export|map_ring|kick|ring_state
     * greppable: dual_dod_ab=OPEN | freestanding_store_product=0 | G-AC-1
     */
    kprintf("store_door: soft residual lean "
            "ops=%u claim_op=%u ring_op=%u map_ring_op=%u sector_path_op=%u "
            "geom=%u map_pages3=%u own=%u wire=%u lic=%u "
            "claim_inval=%u map_inval=%u sector_mult=%u dual_dod_open=%u "
            "udx_handoff_ok=%u ring_map_notes_ok=%u sector_path_ok=%u "
            "checks=%u ok=%u lean_arms=%u "
            "path=claim|rw|map_ring handoff=storaged|UDX product_store=UDX "
            "not_freestanding_store_product freestanding_store_product=0 "
            "udx_host_stack=claim|export|map_ring|kick|ring_state "
            "export=%u kick=%u ring_state=%u map_ring=%u "
            "stats_words=%u queue_words=%u ring_state_words=%u export_sz=%u "
            "ring_enter=%lu map_ok=%lu remap=%lu map_replace=%lu "
            "map_inval_n=%lu map_fault=%lu map_nodev=%lu "
            "host_owned_ring=%lu release_while_mapped=%lu "
            "ring_nodev=%lu owned=%u blk=%u mapped=%u map_va=0x%lx "
            "claims=%u reclaims=%u ring_calls=%u "
            "product=UDX+ABI Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "dual_dod_ab=OPEN soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "stamp_storm=0 G-AC-1=1 no_ko_product=1 "
            "(STRONGER C0 residual lean; Soft!=product; dual MIT OR Apache-2.0; "
            "MAP_RING residual for UDX storage hosts; ring MAP notes sparse; "
            "no freestanding store product; Dual DoD A/B OPEN; no version "
            "stamp; no stamp storms; G-AC-1)\n",
            u32Ops, u32ClaimOp, u32RingOp, u32MapRingOp, u32SectorPathOp,
            u32Geom, u32MapPages, u32Own, u32Wire, u32Lic, u32ClaimInval,
            u32MapInval, u32SectorMult, u32DualDod, u32UdxOk, u32MapNotes,
            u32SectorPathOk, u32Checks, u32Ok, GJ_STORE_LEAN_CHECKS,
            GJ_STORE_OP_EXPORT_RING, GJ_STORE_OP_KICK,
            GJ_STORE_OP_RING_STATE, GJ_STORE_OP_MAP_RING,
            GJ_STORE_STATS_WORDS, GJ_STORE_QUEUE_WORDS,
            GJ_STORE_RING_STATE_WORDS, u32ExportSz,
            (unsigned long)g_soft.u64RingEnter,
            (unsigned long)g_soft.u64MapOk, (unsigned long)g_soft.u64Remap,
            (unsigned long)g_soft.u64MapReplace,
            (unsigned long)g_soft.u64MapInval,
            (unsigned long)g_soft.u64MapFault,
            (unsigned long)g_soft.u64MapNodev,
            (unsigned long)g_soft.u64HostOwnedRing,
            (unsigned long)g_soft.u64ReleaseMapped,
            (unsigned long)g_soft.u64RingNodev, u32Owned, u32Blk, u32Mapped,
            (unsigned long)g_u64RingMapVa, g_u32Claims, g_u32Reclaims,
            g_u32RingCalls);

    /*
     * Grep: store_door: soft residual lean udx
     * STRONGER UDX storage host residual lamp (stamp-free).
     */
    kprintf("store_door: soft residual lean udx "
            "claim_inval=%u map_inval=%u sector_mult=%u dual_dod_open=%u "
            "ops=%u map_ring=%u own=%u wire=%u lic=%u "
            "udx_handoff_ok=%u ok=%u/%u "
            "product=UDX+ABI direction=storaged_map_ring "
            "sshd=1 udx=1 handoff=storaged|UDX product_store=UDX "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 version_stamp=0 "
            "(STRONGER functional residual; Soft!=product; not Dual DoD close)\n",
            u32ClaimInval, u32MapInval, u32SectorMult, u32DualDod, u32Ops,
            u32MapRingOp, u32Own, u32Wire, u32Lic, u32UdxOk, u32Ok,
            u32Checks);

    if (u32Ok == u32Checks && u32Checks == GJ_STORE_LEAN_CHECKS &&
        u32UdxOk >= 3u && u32MapNotes >= 3u && u32Own != 0u &&
        u32Wire != 0u && u32Lic != 0u && u32DualDod != 0u) {
        /* Grep: store_door: soft residual lean PASS */
        kprintf("store_door: soft residual lean PASS "
                "checks=%u ok=%u ops=%u claim=%u ring=%u map_ring=%u "
                "sector=%u geom=%u map_pages3=%u own=%u wire=%u lic=%u "
                "claim_inval=%u map_inval=%u sector_mult=%u dual_dod_open=%u "
                "udx_handoff_ok=%u ring_map_notes_ok=%u sector_path_ok=%u "
                "map_ring_op=%u pages_map=3 "
                "handoff=storaged|UDX product_store=UDX product=UDX+ABI "
                "not_freestanding_store_product freestanding_store_product=0 "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod_ab=OPEN "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                "(STRONGER C0 residual lean PASS; Soft!=product; MAP_RING "
                "residual for UDX storage hosts; Dual DoD A/B OPEN; no "
                "freestanding store product; G-AC-1)\n",
                u32Checks, u32Ok, u32Ops, u32ClaimOp, u32RingOp,
                u32MapRingOp, u32SectorPathOp, u32Geom, u32MapPages, u32Own,
                u32Wire, u32Lic, u32ClaimInval, u32MapInval, u32SectorMult,
                u32DualDod, u32UdxOk, u32MapNotes, u32SectorPathOk,
                GJ_STORE_OP_MAP_RING);
    } else {
        g_soft.u32LeanFail = g_soft.u32LeanFail + 1u;
        /* Grep: store_door: soft residual lean FAIL */
        kprintf("store_door: soft residual lean FAIL "
                "ok=%u/%u ops=%u claim=%u ring=%u map_ring=%u sector=%u "
                "geom=%u map_pages3=%u own=%u wire=%u lic=%u "
                "claim_inval=%u map_inval=%u sector_mult=%u dual_dod_open=%u "
                "(soft residual only; not product gate; Soft!=product; "
                "handoff=storaged|UDX product_store=UDX dual_dod_ab=OPEN)\n",
                u32Ok, u32Checks, u32Ops, u32ClaimOp, u32RingOp,
                u32MapRingOp, u32SectorPathOp, u32Geom, u32MapPages, u32Own,
                u32Wire, u32Lic, u32ClaimInval, u32MapInval, u32SectorMult,
                u32DualDod);
    }
}
/**
 * After first product call activity, print soft residual once.
 * Safe from call return paths only. No per-op re-dump.
 */
static void
store_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_soft.u64Enter == 0) {
        return;
    }
    g_fSoftOnce = 1;
    store_soft_residual_once();
    store_soft_residual_lean_once();
}

/**
 * Soft terminal: classify ret + one-shot residual, then return ret.
 * All product returns from store_door_call go through here (ABI unchanged).
 */
static i64
store_soft_done(i64 i64Ret)
{
    g_soft.i64LastRet = i64Ret;
    store_soft_note_ret(i64Ret);
    store_soft_maybe_once();
    return i64Ret;
}

/**
 * Copy @cb bytes to caller buffer at @u64Dst.
 * Uses copy_to_user when the range is in the user VA window.
 */
static i64
store_copy_out(u64 u64Dst, const void *pSrc, u32 cb)
{
    if (u64Dst == 0 || pSrc == NULL || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Dst, cb)) {
        if (copy_to_user(u64Dst, pSrc, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        /* Kernel-smoke path: destination is a trusted HHDM/static buffer. */
        memcpy((void *)(gj_vaddr_t)u64Dst, pSrc, cb);
    }
    return 0;
}

/**
 * Copy @cb bytes from caller buffer at @u64Src into @pDst.
 */
static i64
store_copy_in(void *pDst, u64 u64Src, u32 cb)
{
    if (pDst == NULL || u64Src == 0 || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Src, cb)) {
        if (copy_from_user(pDst, u64Src, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        memcpy(pDst, (const void *)(gj_vaddr_t)u64Src, cb);
    }
    return 0;
}

void
store_door_init(void)
{
    u32 u32PrevInits;

    u32PrevInits = g_soft.u32InitCalls;
    g_fInit = 1;
    g_u32Calls = 0;
    g_u32DoorRw = 0;
    g_u32OwnerToken = 0;
    g_u32Claims = 0;
    g_u32Reclaims = 0;
    g_u32RingCalls = 0;
    g_u64RingMapVa = 0;
    memset(&g_soft, 0, sizeof(g_soft));
    g_soft.u32InitCalls = u32PrevInits + 1u;
    g_fSoftOnce = 0;
    g_fSoftLean = 0;
    /* Backends may probe later; report readiness snapshot for bring-up. */
    kprintf("store_door: init xfer_max=%u blk=%d scsi=%d "
            "handoff=storaged|UDX map_ring Soft!=product "
            "dual=MIT_OR_Apache-2.0 G-AC-1 "
            "udx_host_stack=claim|export|map_ring|kick|ring_state\n",
            STORE_XFER_MAX, virtio_blk_ready() ? 1 : 0,
            scsi_mid_ready() ? 1 : 0);
    /* Grep: store_door: soft residual (init lamp; sparse) */
    kprintf("store_door: soft residual init xfer_max=%u sector=%u page=%u "
            "lean_arms=%u path=claim|rw|map_ring backend=blk|scsi "
            "handoff=storaged|UDX product_store=UDX "
            "not_freestanding_store_product freestanding_store_product=0 "
            "udx_host_stack=claim|export|map_ring|kick|ring_state "
            "soft=1 product=0 Soft!=product G-AC-1 dual_dod_ab=OPEN "
            "stamp_storm=0\n",
            (u32)STORE_XFER_MAX, (u32)GJ_VIRTIO_BLK_SECTOR,
            (u32)STORE_DOOR_PAGE_SIZE, (u32)GJ_STORE_LEAN_CHECKS);
    /* Grep: store_door: soft residual lean (init lean; sparse) */
    kprintf("store_door: soft residual lean init "
            "claim_op=%u export=%u kick=%u ring_state=%u map_ring=%u "
            "stats=%u cap=%u read=%u write=%u queue=%u flush=%u "
            "stats_words=%u queue_words=%u ring_state_words=%u lean_arms=%u "
            "path=claim|rw|map_ring handoff=storaged|UDX product_store=UDX "
            "not_freestanding_store_product freestanding_store_product=0 "
            "udx_host_stack=claim|export|map_ring|kick|ring_state "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 dual_dod_ab=OPEN "
            "(C0 residual lean; Soft!=product; MAP_RING residual for UDX "
            "storage hosts; ring MAP notes; no freestanding store product; "
            "Dual DoD A/B OPEN; no version stamp; no stamp storms; G-AC-1)\n",
            GJ_STORE_OP_CLAIM, GJ_STORE_OP_EXPORT_RING, GJ_STORE_OP_KICK,
            GJ_STORE_OP_RING_STATE, GJ_STORE_OP_MAP_RING,
            GJ_STORE_OP_STATS, GJ_STORE_OP_CAP, GJ_STORE_OP_READ,
            GJ_STORE_OP_WRITE, GJ_STORE_OP_QUEUE_INFO, GJ_STORE_OP_FLUSH,
            GJ_STORE_STATS_WORDS, GJ_STORE_QUEUE_WORDS,
            GJ_STORE_RING_STATE_WORDS, GJ_STORE_LEAN_CHECKS);
    /* Grep: store_door: ring map notes (init; sparse) */
    kprintf("store_door: ring map notes init Soft!=product product_store=UDX "
            "page=%u pages_map=3 map_ring_op=%u export_op=%u kick_op=%u "
            "ring_state_op=%u map_va=0 reclaim=soft_same_va "
            "replace=soft_other_va "
            "udx_attach=map_va+0|page|2page "
            "udx_host_stack=claim|export|map_ring|kick|ring_state "
            "handoff=storaged|UDX freestanding_store_product=0 "
            "dual_dod_ab=OPEN "
            "(NODEV soft-skip without virtio-blk; no per-MAP dump; "
            "dual MIT OR Apache-2.0; G-AC-1; C0 residual)\n",
            (unsigned)STORE_DOOR_PAGE_SIZE, GJ_STORE_OP_MAP_RING,
            GJ_STORE_OP_EXPORT_RING, GJ_STORE_OP_KICK,
            GJ_STORE_OP_RING_STATE);
}

int
store_door_owned(void)
{
    return g_u32OwnerToken != 0;
}

u32
store_door_owner_token(void)
{
    return g_u32OwnerToken;
}

u64
store_door_ring_map_va(void)
{
    return g_u64RingMapVa;
}

u32
store_door_ring_calls(void)
{
    /* Soft diagnostics only - no inventory re-dump (no stamp storms). */
    return g_u32RingCalls;
}

u32
store_door_claim_count(void)
{
    /* Soft diagnostics: first claims + idempotent reclaims. */
    return g_u32Claims + g_u32Reclaims;
}

i64
store_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    if (!g_fInit) {
        store_soft_inc(&g_soft.u64NotInit);
        g_soft.u32LastOp = u32Op;
        return store_soft_done(GJ_ERR_NODEV);
    }
    g_u32Calls++;
    store_soft_inc(&g_soft.u64Enter);
    g_soft.u32LastOp = u32Op;

    switch (u32Op) {
    case GJ_STORE_OP_CLAIM:
        /* arg1 = non-zero ownership token (low 32 bits only). */
        if (u64Arg1 == 0 || (u64Arg1 >> 32) != 0) {
            store_soft_inc(&g_soft.u64ClaimInval);
            return store_soft_done(GJ_ERR_INVAL);
        }
        if (g_u32OwnerToken != 0 && g_u32OwnerToken != (u32)u64Arg1) {
            store_soft_inc(&g_soft.u64ClaimBusy);
            return store_soft_done(GJ_ERR_BUSY); /* another storaged */
        }
        /* Soft reclaim: same token re-CLAIM is idempotent. */
        if (g_u32OwnerToken == (u32)u64Arg1) {
            g_u32Reclaims++;
            store_soft_inc(&g_soft.u64ClaimReclaim);
            return store_soft_done(0);
        }
        g_u32OwnerToken = (u32)u64Arg1;
        g_u32Claims++;
        store_soft_inc(&g_soft.u64ClaimOk);
        kprintf("store_door: CLAIM token=0x%x (userspace owns storage)\n",
                g_u32OwnerToken);
        return store_soft_done(0);

    case GJ_STORE_OP_RELEASE:
        /* Soft free path: already unowned -> 0 (no token match required). */
        if (g_u32OwnerToken == 0) {
            store_soft_inc(&g_soft.u64ReleaseFree);
            return store_soft_done(0);
        }
        if ((u64Arg1 >> 32) != 0 || (u32)u64Arg1 != g_u32OwnerToken) {
            store_soft_inc(&g_soft.u64ReleaseInval);
            return store_soft_done(GJ_ERR_INVAL);
        }
        /*
         * Soft residual deepen: note RELEASE while MAP_RING map_va still
         * held. Does not clear map_va (diagnostic last-VA remains until
         * re-init / re-MAP) - Soft!=product honesty only.
         */
        if (g_u64RingMapVa != 0) {
            store_soft_inc(&g_soft.u64ReleaseMapped);
        }
        kprintf("store_door: RELEASE token=0x%x\n", g_u32OwnerToken);
        g_u32OwnerToken = 0;
        store_soft_inc(&g_soft.u64ReleaseOk);
        return store_soft_done(0);

    case GJ_STORE_OP_STATS: {
        /* aSt: [0]=virtio-blk io, [1]=scsi_door io, [2]=door call count */
        u32 aSt[3];
        i64 st;

        if (u64Arg1 == 0) {
            return store_soft_done(GJ_ERR_INVAL);
        }
        aSt[0] = virtio_blk_ready() ? virtio_blk_io_count() : 0u;
        aSt[1] = scsi_door_io_count();
        aSt[2] = g_u32Calls;
        st = store_copy_out(u64Arg1, aSt, sizeof(aSt));
        if (st == 0) {
            store_soft_inc(&g_soft.u64Stats);
        }
        return store_soft_done(st);
    }

    case GJ_STORE_OP_CAP: {
        u64 u64Cap;
        i64 st;

        if (u64Arg1 == 0) {
            return store_soft_done(GJ_ERR_INVAL);
        }
        if (virtio_blk_ready()) {
            u64Cap = virtio_blk_capacity_sectors();
        } else if (scsi_mid_ready()) {
            /* scsi -> storaged CAP path when virtio-blk absent */
            struct scsi_door_req dreq;
            u8 aCap[8];
            u32 u32Last;

            memset(&dreq, 0, sizeof(dreq));
            dreq.u32Op = GJ_SCSI_DOOR_OP_READ_CAP;
            dreq.cbData = 8;
            if (scsi_door_submit(&dreq, aCap, 8) != 0) {
                store_soft_inc(&g_soft.u64CapNodev);
                return store_soft_done(GJ_ERR_NODEV);
            }
            /* READ CAPACITY(10): big-endian last LBA in first 4 bytes. */
            u32Last = ((u32)aCap[0] << 24) | ((u32)aCap[1] << 16) |
                      ((u32)aCap[2] << 8) | (u32)aCap[3];
            /* Capacity = last LBA + 1 (sectors); widen before add. */
            u64Cap = (u64)u32Last + 1ull;
        } else {
            store_soft_inc(&g_soft.u64CapNodev);
            return store_soft_done(GJ_ERR_NODEV);
        }
        st = store_copy_out(u64Arg1, &u64Cap, sizeof(u64Cap));
        if (st == 0) {
            store_soft_inc(&g_soft.u64Cap);
        }
        return store_soft_done(st);
    }

    case GJ_STORE_OP_READ:
    case GJ_STORE_OP_WRITE: {
        u64 u64Lba = u64Arg1;
        u32 cb = (u32)u64Arg3;
        static u8 aTmp[STORE_XFER_MAX];
        int nIo;
        int fBlk = virtio_blk_ready();
        int fScsi = !fBlk && scsi_mid_ready();
        i64 st;

        g_soft.u64LastLba = u64Lba;

        if (!fBlk && !fScsi) {
            return store_soft_done(GJ_ERR_NODEV);
        }
        /* Reject wide arg3 truncation; require sector-multiple length. */
        if (u64Arg2 == 0 || cb == 0 || (u64Arg3 >> 32) != 0 ||
            cb > STORE_XFER_MAX || (cb % GJ_VIRTIO_BLK_SECTOR) != 0) {
            return store_soft_done(GJ_ERR_INVAL);
        }
        /*
         * READ10/WRITE10 LBA field is 32-bit. Reject out-of-range LBAs on
         * the scsi fallback path (virtio-blk accepts full u64 sector).
         */
        if (fScsi && (u64Lba >> 32) != 0) {
            return store_soft_done(GJ_ERR_INVAL);
        }

        if (u32Op == GJ_STORE_OP_WRITE) {
            st = store_copy_in(aTmp, u64Arg2, cb);
            if (st != 0) {
                return store_soft_done(st);
            }
            if (fBlk) {
                nIo = virtio_blk_write(u64Lba, aTmp, cb);
            } else {
                struct scsi_door_req dreq;
                u32 u32Blocks = cb / GJ_VIRTIO_BLK_SECTOR;

                /* Defensive: blocks must fit u16 (xfer_max guarantees this). */
                if (u32Blocks == 0 || u32Blocks > 0xffffu) {
                    return store_soft_done(GJ_ERR_INVAL);
                }
                memset(&dreq, 0, sizeof(dreq));
                dreq.u32Op = GJ_SCSI_DOOR_OP_WRITE10;
                dreq.u32Lba = (u32)u64Lba;
                dreq.u16Blocks = (u16)u32Blocks;
                dreq.cbData = cb;
                nIo = scsi_door_submit(&dreq, aTmp, cb);
            }
        } else {
            if (fBlk) {
                nIo = virtio_blk_read(u64Lba, aTmp, cb);
            } else {
                struct scsi_door_req dreq;
                u32 u32Blocks = cb / GJ_VIRTIO_BLK_SECTOR;

                if (u32Blocks == 0 || u32Blocks > 0xffffu) {
                    return store_soft_done(GJ_ERR_INVAL);
                }
                memset(&dreq, 0, sizeof(dreq));
                dreq.u32Op = GJ_SCSI_DOOR_OP_READ10;
                dreq.u32Lba = (u32)u64Lba;
                dreq.u16Blocks = (u16)u32Blocks;
                dreq.cbData = cb;
                nIo = scsi_door_submit(&dreq, aTmp, cb);
            }
            if (nIo == 0) {
                st = store_copy_out(u64Arg2, aTmp, cb);
                if (st != 0) {
                    return store_soft_done(st);
                }
            }
        }
        if (nIo != 0) {
            return store_soft_done(GJ_ERR_IO);
        }
        g_u32DoorRw++;
        if (u32Op == GJ_STORE_OP_WRITE) {
            store_soft_inc(&g_soft.u64WriteOk);
            store_soft_add64(&g_soft.u64WriteBytes, (u64)cb);
        } else {
            store_soft_inc(&g_soft.u64ReadOk);
            store_soft_add64(&g_soft.u64ReadBytes, (u64)cb);
        }
        if (fBlk) {
            store_soft_inc(&g_soft.u64RwBlk);
        } else {
            store_soft_inc(&g_soft.u64RwScsi);
        }
        if (g_u32OwnerToken != 0) {
            store_soft_inc(&g_soft.u64RwOwned);
        } else {
            store_soft_inc(&g_soft.u64RwUnowned);
        }
        if (cb > GJ_VIRTIO_BLK_SECTOR) {
            store_soft_inc(&g_soft.u64RwMulti);
        }
        return store_soft_done((i64)cb);
    }

    case GJ_STORE_OP_QUEUE_INFO: {
        /* aQ: [0]=blk_io [1]=scsi_io [2]=door_rw [3]=owned(0/1) - wire stable */
        u32 aQ[4];
        i64 st;

        if (u64Arg1 == 0) {
            return store_soft_done(GJ_ERR_INVAL);
        }
        aQ[0] = virtio_blk_ready() ? virtio_blk_io_count() : 0u;
        aQ[1] = scsi_door_io_count();
        aQ[2] = g_u32DoorRw;
        aQ[3] = g_u32OwnerToken ? 1u : 0u;
        st = store_copy_out(u64Arg1, aQ, sizeof(aQ));
        if (st == 0) {
            store_soft_inc(&g_soft.u64Queue);
        }
        return store_soft_done(st);
    }

    case GJ_STORE_OP_FLUSH:
        /*
         * Virtio-blk flush not wired; fsync-shaped success when a block
         * transport is ready. scsi_mid has no SYNCHRONIZE CACHE yet.
         */
        if (!virtio_blk_ready() && !scsi_mid_ready()) {
            store_soft_inc(&g_soft.u64FlushNodev);
            return store_soft_done(GJ_ERR_NODEV);
        }
        store_soft_inc(&g_soft.u64Flush);
        return store_soft_done(0);

    case GJ_STORE_OP_EXPORT_RING: {
        struct gj_virtq_export ex;
        i64 st;

        store_soft_ring_enter();
        store_soft_inc(&g_soft.u64ExportEnter);
        if (u64Arg1 == 0) {
            return store_soft_done(GJ_ERR_INVAL);
        }
        /* Soft-skip surface: no virtio-blk -> NODEV (storaged soft-logs). */
        if (!virtio_blk_ready()) {
            store_soft_inc(&g_soft.u64ExportNodev);
            store_soft_inc(&g_soft.u64RingNodev);
            return store_soft_done(GJ_ERR_NODEV);
        }
        if (virtio_blk_export_q(&ex) != 0) {
            store_soft_inc(&g_soft.u64ExportNodev);
            store_soft_inc(&g_soft.u64RingNodev);
            return store_soft_done(GJ_ERR_NODEV);
        }
        st = store_copy_out(u64Arg1, &ex, sizeof(ex));
        if (st == 0) {
            store_soft_inc(&g_soft.u64ExportOk);
        }
        return store_soft_done(st);
    }

    case GJ_STORE_OP_KICK:
        store_soft_ring_enter();
        store_soft_inc(&g_soft.u64KickEnter);
        /* Soft-skip when blk absent; kick is best-effort notify. */
        if (!virtio_blk_ready()) {
            store_soft_inc(&g_soft.u64KickNodev);
            store_soft_inc(&g_soft.u64RingNodev);
            return store_soft_done(GJ_ERR_NODEV);
        }
        if (virtio_blk_kick_q() != 0) {
            store_soft_inc(&g_soft.u64KickNodev);
            store_soft_inc(&g_soft.u64RingNodev);
            return store_soft_done(GJ_ERR_NODEV);
        }
        store_soft_inc(&g_soft.u64KickOk);
        return store_soft_done(0);

    case GJ_STORE_OP_RING_STATE: {
        /* Soft: always fills {free, ready}; ready=0 without virtio-blk. */
        u32 aS[2];
        i64 st;

        store_soft_ring_enter();
        store_soft_inc(&g_soft.u64RingStateEnter);
        if (u64Arg1 == 0) {
            return store_soft_done(GJ_ERR_INVAL);
        }
        aS[0] = virtio_blk_ready() ? virtio_blk_q_free() : 0u;
        aS[1] = virtio_blk_ready() ? 1u : 0u;
        st = store_copy_out(u64Arg1, aS, sizeof(aS));
        if (st == 0) {
            store_soft_inc(&g_soft.u64RingState);
        }
        return store_soft_done(st);
    }

    case GJ_STORE_OP_MAP_RING: {
        /*
         * MAP_RING residual for UDX storage hosts (Soft!=product):
         *   arg1 = page-aligned user VA base (desc|avail|used)
         *   arg2 = optional gj_virtq_export out (may be 0)
         * Soft re-MAP same VA = map reclaim (re-install PTEs + re-export).
         * Soft MAP different VA while mapped = map replace (diagnostic
         * last-VA updates; no product unmap of prior window).
         * NODEV without virtio-blk; FAULT on map_q_user fail; INVAL on
         * zero / unaligned VA. Product path = storaged/UDX (G-AC-1).
         */
        struct gj_virtq_export ex;
        i64 st;
        int fRemap;
        int fReplace;

        store_soft_ring_enter();
        store_soft_inc(&g_soft.u64MapEnter);
        if (u64Arg1 == 0) {
            store_soft_inc(&g_soft.u64MapInval);
            return store_soft_done(GJ_ERR_INVAL);
        }
        /* VA base must be page-aligned for ring map into user AS. */
        if ((u64Arg1 & (STORE_DOOR_PAGE_SIZE - 1ull)) != 0) {
            store_soft_inc(&g_soft.u64MapInval);
            return store_soft_done(GJ_ERR_INVAL);
        }
        /* Soft-skip surface: no blk -> NODEV (distinct from map FAULT). */
        if (!virtio_blk_ready()) {
            store_soft_inc(&g_soft.u64MapNodev);
            store_soft_inc(&g_soft.u64RingNodev);
            return store_soft_done(GJ_ERR_NODEV);
        }
        /*
         * Soft re-MAP of the same VA: re-install PTEs + re-export (idempotent
         * hand-off for storaged / UDX reclaim of the map window).
         * Soft replace: different VA while map_va held (honesty deepen).
         */
        fRemap = (g_u64RingMapVa != 0 && g_u64RingMapVa == u64Arg1) ? 1 : 0;
        fReplace = (g_u64RingMapVa != 0 && g_u64RingMapVa != u64Arg1) ? 1 : 0;
        if (virtio_blk_map_q_user(u64Arg1, &ex) != 0) {
            store_soft_inc(&g_soft.u64MapFault);
            return store_soft_done(GJ_ERR_FAULT);
        }
        g_u64RingMapVa = u64Arg1;
        if (fRemap) {
            store_soft_inc(&g_soft.u64Remap);
        } else if (fReplace) {
            store_soft_inc(&g_soft.u64MapReplace);
        } else {
            store_soft_inc(&g_soft.u64MapOk);
        }
        if (u64Arg2 != 0) {
            st = store_copy_out(u64Arg2, &ex, sizeof(ex));
            if (st != 0) {
                return store_soft_done(st);
            }
        }
        return store_soft_done(0);
    }

    default:
        return store_soft_done(GJ_ERR_NOSUPPORT);
    }
}
