/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * User pointer validation + copy with STAC/CLAC when SMAP is enabled.
 * G-PTR-*: range must sit in product user window, be present, and U=1.
 *
 * Lean residual for DDI/UDX (exclusive this TU; Soft!=product dual):
 *   Functional: user_range_ok + mapped_access(W|COW) + page-chunk STAC/CLAC
 *     copy_from/to_user + load/store_u32. Fail closed FAULT/INVAL (G-PTR-3).
 *   DDI consumers (G-PTR for GJ_SYS_DDI): ddi_door ddi_copy_out /
 *     MAP_BAR arg3 dual-use (user_range_ok selects map_note* vs VA hint);
 *     GET out-struct via copy_to_user under host process AS.
 *   Hosts: ddi_host_gj / rtl8168_udx / xhci_udx - userspace dual-license only.
 *   Soft inventory only - never product SEH / full SMAP / MMIO_FRAME mint.
 *   Product mint OPEN; Dual DoD A/B OPEN; G-AC-1 (no .ko product AC).
 *   Dual MIT OR Apache-2.0. No version stamp. No stamp storms.
 *
 * C3 residual deepen (security/isolation; behavior > soft lamp):
 *   1) Dual-side page-boundary chunk clamp under STAC/CLAC so one AC window
 *      never spans past either src or dst page end (tightens copy_to_user where
 *      user is destination; single-side src clamp left user pages under one
 *      STAC). Chunked path fail-closed if geometry yields zero chunk.
 *   2) Scalar load/store_u32: page-cross uses per-byte STAC/CLAC so one AC
 *      window never spans two user pages (dual_page parity for u32 edges).
 *   Pure geometry self-check in lean residual. Soft!=product.
 *   greppable: user_copy: soft chunk dual_page=1 | soft residual lean c3=1
 *   greppable: user_copy: soft scalar dual_page=1 | soft residual lean scalar_x=1
 *
 * greppable soft prefixes (lean residual; soft != product):
 *   "user_copy: soft honesty ..."         explicit non-claims
 *   "user_copy: soft inventory ..."       rollup (ok/fault/inval + bytes)
 *   "user_copy: soft range ..."           range-ok fail reason axes
 *   "user_copy: soft map ..."             map fail + read/write intent
 *   "user_copy: soft chunk ..."           STAC/CLAC + dual_page chunk/page peaks
 *   "user_copy: soft zero ..."            zero-length early-return tallies
 *   "user_copy: soft path ..."            ladder + OPEN honesty lamps
 *   "user_copy: soft ddi ..."             DDI/UDX G-PTR residual honesty
 *   "user_copy: soft residual lean ..."   lean self-check + PASS|FAIL (c3)
 *   "user_copy: soft deepen ..."          lean residual areas + surf catalog
 */
#include <gj/config.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

/* x86_64 PTE bits used for product user-map checks (match vmm.c). */
#define GJ_USER_PTE_P   (1ull << 0)
#define GJ_USER_PTE_W   (1ull << 1)
#define GJ_USER_PTE_U   (1ull << 2)
#define GJ_USER_PTE_COW (1ull << 9) /* software COW leaf (vmm PTE_COW) */

/*
 * Soft inventory greppable area count (honesty..deepen; lean residual).
 * Includes DDI residual + residual lean self-check. Grep: soft residual lean
 */
#define USER_COPY_SOFT_AREAS 10u

/*
 * Lean return-surface bit lamps (surf=0x... on soft deepen).
 * greppable: user_copy: soft deepen | user_copy: soft residual lean
 */
#define USER_COPY_SOFT_SURF_HONESTY   (1u << 0)
#define USER_COPY_SOFT_SURF_INVENTORY (1u << 1)
#define USER_COPY_SOFT_SURF_RANGE     (1u << 2)
#define USER_COPY_SOFT_SURF_MAP       (1u << 3)
#define USER_COPY_SOFT_SURF_CHUNK     (1u << 4)
#define USER_COPY_SOFT_SURF_ZERO      (1u << 5)
#define USER_COPY_SOFT_SURF_PATH      (1u << 6)
#define USER_COPY_SOFT_SURF_DDI       (1u << 7)
#define USER_COPY_SOFT_SURF_LEAN      (1u << 8)
#define USER_COPY_SOFT_SURF_DEEPEN    (1u << 9)
#define USER_COPY_SOFT_SURF_CATALOG                                              \
    (USER_COPY_SOFT_SURF_HONESTY | USER_COPY_SOFT_SURF_INVENTORY |             \
     USER_COPY_SOFT_SURF_RANGE | USER_COPY_SOFT_SURF_MAP |                     \
     USER_COPY_SOFT_SURF_CHUNK | USER_COPY_SOFT_SURF_ZERO |                    \
     USER_COPY_SOFT_SURF_PATH | USER_COPY_SOFT_SURF_DDI |                      \
     USER_COPY_SOFT_SURF_LEAN | USER_COPY_SOFT_SURF_DEEPEN)

/*
 * Soft dual-use sizes that DDI door probes via user_range_ok (layout honesty).
 * Kept local - never includes ddi_door.h (layering; avoid cycle). Soft only.
 * gj_ddi_map_note is packed and currently 40 bytes; floor for dual-use detect.
 */
#define USER_COPY_SOFT_DDI_MAP_NOTE_MIN  32u
#define USER_COPY_SOFT_DDI_DEV_INFO_MIN  64u

static int                       g_fSmapOn;
static struct gj_user_copy_stats g_stats;

/*
 * Soft inventory extras (file-local - not hard product gates).
 * greppable: user_copy: soft
 */
static u64 g_u64SoftPeakFrom;      /* max successful copy_from_user cb */
static u64 g_u64SoftPeakTo;        /* max successful copy_to_user cb */
static u64 g_u64SoftLastFrom;      /* last successful copy_from_user cb */
static u64 g_u64SoftLastTo;        /* last successful copy_to_user cb */
static u64 g_u64SoftInventoryLogs; /* soft_inventory_log emissions */

/* Range-ok fail reason axes (soft; wrap OK). */
static u64 g_u64SoftRangeOversize;
static u64 g_u64SoftRangeBelowBase;
static u64 g_u64SoftRangeAboveEnd;
static u64 g_u64SoftRangeOverflow;
static u64 g_u64SoftRangeEndBeyond;

/* Map fail reason axes (soft; wrap OK). */
static u64 g_u64SoftMapNotPresent;
static u64 g_u64SoftMapNotUser;
static u64 g_u64SoftMapWriteRo;   /* write intent, !W and !COW */
static u64 g_u64SoftMapReadOk;    /* mapped_access read intent soft ok */
static u64 g_u64SoftMapWriteOk;   /* mapped_access write intent soft ok */
static u64 g_u64SoftMapReadCall;  /* mapped_access read-intent calls */
static u64 g_u64SoftMapWriteCall; /* mapped_access write-intent calls */

/* Zero-length early returns + chunk/page soft peaks. */
static u64 g_u64SoftZeroFrom;
static u64 g_u64SoftZeroTo;
static u64 g_u64SoftZeroLoad;   /* load/store never zero-cb; reserved */
static u64 g_u64SoftPeakChunks; /* max chunks in one copy_raw_chunked */
static u64 g_u64SoftLastChunks; /* last transfer chunk count */
static u64 g_u64SoftPeakPages;  /* max pages spanned (soft estimate) */
static u64 g_u64SoftLastPages;  /* last transfer pages spanned */
/* C3 dual-side page clamp soft tallies (src vs dst page-end min). */
static u64 g_u64SoftDualBound;  /* chunks where src/dst page remainders differ */
static u64 g_u64SoftDualClamp; /* chunks shortened vs single-side src clamp */
static u64 g_u64SoftChunkGeomFail; /* zero-chunk under cbLeft>0 (fail closed) */

/* C3 scalar dual_page soft tallies (load/store_u32 page-cross path). */
static u64 g_u64SoftScalarDirect; /* single-STAC u32 wholly in one page */
static u64 g_u64SoftScalarCross;  /* page-crossing u32 -> per-byte STAC */
static u64 g_u64SoftScalarBytes;  /* byte touches under scalar-cross path */

/* Lean residual self-check tallies (DDI/G-PTR geometry; soft only). */
static u64 g_u64SoftLeanRuns;
static u64 g_u64SoftLeanOk;
static u64 g_u64SoftLeanFail;
static u8  g_fSoftLeanOnce; /* one-shot PASS|FAIL lamp */

static void   user_copy_soft_inventory_log(void);
static void   user_copy_soft_note_from(size_t cb);
static void   user_copy_soft_note_to(size_t cb);
static void   user_copy_soft_note_chunked(size_t cb, u64 u64Chunks);
static u32    user_copy_soft_lean_selfcheck(void);
static size_t user_copy_chunk_len(const void *pDst, const void *pSrc,
                                  size_t cbLeft);
static int    user_copy_u32_page_cross(u64 u64Uaddr);
static int    copy_raw_chunked(void *pDst, const void *pSrc, size_t cb);

/**
 * First page-chunk length under dual-side clamp (C3 residual deepen).
 * STAC window never spans past either src or dst page end.
 * Pure geometry - no memory access. Soft!=product.
 * greppable: user_copy: soft chunk dual_page=1
 */
static size_t
user_copy_chunk_len(const void *pDst, const void *pSrc, size_t cbLeft)
{
    size_t cbOffS;
    size_t cbOffD;
    size_t cbChunkS;
    size_t cbChunkD;
    size_t cbChunk;

    if (cbLeft == 0) {
        return 0;
    }
    cbOffS = (size_t)((u64)(gj_vaddr_t)pSrc & (u64)(GJ_PAGE_SIZE - 1));
    cbOffD = (size_t)((u64)(gj_vaddr_t)pDst & (u64)(GJ_PAGE_SIZE - 1));
    cbChunkS = (size_t)GJ_PAGE_SIZE - cbOffS;
    cbChunkD = (size_t)GJ_PAGE_SIZE - cbOffD;
    cbChunk = cbChunkS < cbChunkD ? cbChunkS : cbChunkD;
    if (cbChunk > cbLeft) {
        cbChunk = cbLeft;
    }
    /* C3 invariant: one AC window never exceeds a single page. */
    if (cbChunk > (size_t)GJ_PAGE_SIZE) {
        cbChunk = (size_t)GJ_PAGE_SIZE;
    }
    return cbChunk;
}

/**
 * Non-zero if a 4-byte scalar at u64Uaddr spans two pages (C3 geometry).
 * Pure address math - no memory access. Soft!=product.
 * greppable: user_copy: soft scalar dual_page=1
 */
static int
user_copy_u32_page_cross(u64 u64Uaddr)
{
    u64 u64Off;

    u64Off = u64Uaddr & (u64)(GJ_PAGE_SIZE - 1);
    return (u64Off + (u64)sizeof(u32) > (u64)GJ_PAGE_SIZE) ? 1 : 0;
}

/**
 * Lean residual self-check for DDI/G-PTR + C3 dual-page geometry.
 * Never hard-gates boot. Soft!=product. Dual MIT OR Apache-2.0.
 * greppable: user_copy: soft residual lean
 *
 * Checks (pure geometry / API shape - no process AS required):
 *   - user window floor 8 MiB, end 2 GiB exclusive, max == end-base
 *   - empty cb range_ok succeeds; oversize / below-base fail closed
 *   - dual-use DDI note/dev sizes fit inside max copy span
 *   - user_range_ok at BASE with map_note_min size accepts (window only)
 *   - C3 dual-side chunk clamp: src mid / dst mid / both mid (no deref)
 *   - C3 chunk never exceeds one page; empty left -> 0
 *   - C3 scalar page-cross detect: aligned no / page-end-3 yes
 * Returns number of checks that passed (expect USER_COPY_SOFT_LEAN_CHECKS).
 */
#define USER_COPY_SOFT_LEAN_CHECKS 16u

static u32
user_copy_soft_lean_selfcheck(void)
{
    u32 u32Ok = 0;
    u64 u64Max;
    u64 u64SavedFail;
    u64 u64SavedOver;
    u64 u64SavedBelow;
    u64 u64SavedAbove;
    u64 u64SavedOvf;
    u64 u64SavedEnd;
    u8 *pBase;
    u64 u64PageEnd;

    if (g_u64SoftLeanRuns < ~0ull) {
        g_u64SoftLeanRuns++;
    }

    /* Snapshot range-fail axes so self-check probes do not pollute smoke. */
    u64SavedFail = g_stats.u64RangeOkFail;
    u64SavedOver = g_u64SoftRangeOversize;
    u64SavedBelow = g_u64SoftRangeBelowBase;
    u64SavedAbove = g_u64SoftRangeAboveEnd;
    u64SavedOvf = g_u64SoftRangeOverflow;
    u64SavedEnd = g_u64SoftRangeEndBeyond;

    u64Max = (u64)(GJ_USER_VA_END - GJ_USER_VA_BASE);
    if (GJ_USER_VA_BASE == 0x0000000000800000ull &&
        GJ_USER_VA_END == 0x0000000080000000ull &&
        GJ_USER_COPY_MAX == u64Max) {
        u32Ok++;
    }
    /* Empty cb always ok (G-PTR soft). */
    if (user_range_ok(0, 0) != 0 &&
        user_range_ok(GJ_USER_VA_BASE, 0) != 0) {
        u32Ok++;
    }
    /* Below base fail closed. */
    if (user_range_ok(0x1000ull, 16u) == 0) {
        u32Ok++;
    }
    /* Oversize fail closed. */
    if (user_range_ok(GJ_USER_VA_BASE, GJ_USER_COPY_MAX + 1ull) == 0) {
        u32Ok++;
    }
    /* Window accept: BASE + small span wholly inside. */
    if (user_range_ok(GJ_USER_VA_BASE, 16u) != 0) {
        u32Ok++;
    }
    /* End-beyond fail closed (last page past END). */
    if (user_range_ok(GJ_USER_VA_END - 8ull, 16u) == 0) {
        u32Ok++;
    }
    /* DDI dual-use sizes fit user window (map_note / dev_info floors). */
    if ((u64)USER_COPY_SOFT_DDI_MAP_NOTE_MIN <= GJ_USER_COPY_MAX &&
        (u64)USER_COPY_SOFT_DDI_DEV_INFO_MIN <= GJ_USER_COPY_MAX &&
        user_range_ok(GJ_USER_VA_BASE,
                      (u64)USER_COPY_SOFT_DDI_MAP_NOTE_MIN) != 0 &&
        user_range_ok(GJ_USER_VA_BASE,
                      (u64)USER_COPY_SOFT_DDI_DEV_INFO_MIN) != 0) {
        u32Ok++;
    }
    /* Overflow fail closed (va + cb wrap). */
    if (user_range_ok(~0ull - 8ull, 32u) == 0) {
        u32Ok++;
    }

    /*
     * C3 dual-side page-chunk geometry (pure address math; never deref).
     * pBase is a synthetic user-window pointer for offsets only.
     * greppable: user_copy: soft residual lean c3=1 dual_page_chunk=1
     */
    pBase = (u8 *)(gj_vaddr_t)GJ_USER_VA_BASE;
    /* Both near page base: small transfer fits one chunk. */
    if (user_copy_chunk_len(pBase, pBase, 16u) == 16u) {
        u32Ok++;
    }
    /* Src mid-page (off 0xF00): first chunk stops at src page end (0x100). */
    if (user_copy_chunk_len(pBase, pBase + 0xF00u, 0x200u) == 0x100u) {
        u32Ok++;
    }
    /* Dst mid-page: C3 tighten for copy_to_user (user is destination). */
    if (user_copy_chunk_len(pBase + 0xF00u, pBase, 0x200u) == 0x100u) {
        u32Ok++;
    }
    /* Both mid different: min(src remain 0x200, dst remain 0x100) = 0x100. */
    if (user_copy_chunk_len(pBase + 0xF00u, pBase + 0xE00u, 0x300u) ==
        0x100u) {
        u32Ok++;
    }
    /* Equal mid offsets: dual remainders equal -> single-side same as dual. */
    if (user_copy_chunk_len(pBase + 0xF00u, pBase + 0xF00u, 0x200u) ==
        0x100u) {
        u32Ok++;
    }
    /* Empty left -> 0; full page-aligned span clamps to one page max. */
    if (user_copy_chunk_len(pBase, pBase, 0) == 0 &&
        user_copy_chunk_len(pBase, pBase, (size_t)GJ_PAGE_SIZE * 2u) ==
            (size_t)GJ_PAGE_SIZE) {
        u32Ok++;
    }

    /*
     * C3 scalar page-cross geometry (load/store_u32 dual_page parity).
     * greppable: user_copy: soft residual lean c3=1 scalar_x=1
     */
    u64PageEnd = GJ_USER_VA_BASE + (u64)GJ_PAGE_SIZE;
    /* Page-aligned u32: wholly in one page. */
    if (user_copy_u32_page_cross(GJ_USER_VA_BASE) == 0 &&
        user_copy_u32_page_cross(u64PageEnd - 4ull) == 0) {
        u32Ok++;
    }
    /* Offsets PAGE_SIZE-3..PAGE_SIZE-1: 4-byte scalar spans next page. */
    if (user_copy_u32_page_cross(u64PageEnd - 3ull) != 0 &&
        user_copy_u32_page_cross(u64PageEnd - 2ull) != 0 &&
        user_copy_u32_page_cross(u64PageEnd - 1ull) != 0) {
        u32Ok++;
    }

    /* Restore axes so self-check is inventory-silent for smoke greps. */
    g_stats.u64RangeOkFail = u64SavedFail;
    g_u64SoftRangeOversize = u64SavedOver;
    g_u64SoftRangeBelowBase = u64SavedBelow;
    g_u64SoftRangeAboveEnd = u64SavedAbove;
    g_u64SoftRangeOverflow = u64SavedOvf;
    g_u64SoftRangeEndBeyond = u64SavedEnd;

    if (u32Ok == USER_COPY_SOFT_LEAN_CHECKS) {
        if (g_u64SoftLeanOk < ~0ull) {
            g_u64SoftLeanOk++;
        }
    } else if (g_u64SoftLeanFail < ~0ull) {
        g_u64SoftLeanFail++;
    }
    return u32Ok;
}

/**
 * Lean greppable soft inventory (diagnostics only; never hard-gates).
 * Soft != product: not SEH, not full SMAP product close, not DDI mint.
 * No version stamp. Never allocates; safe from SMAP notify / stats get/reset.
 * greppable: user_copy: soft | user_copy: soft residual lean
 */
static void
user_copy_soft_inventory_log(void)
{
    u64 u64FromOk;
    u64 u64FromFault;
    u64 u64FromInval;
    u64 u64ToOk;
    u64 u64ToFault;
    u64 u64ToInval;
    u64 u64LoadOk;
    u64 u64LoadFault;
    u64 u64LoadInval;
    u64 u64StoreOk;
    u64 u64StoreFault;
    u64 u64BytesFrom;
    u64 u64BytesTo;
    u64 u64RangeOkFail;
    u64 u64RangeMapFail;
    u64 u64Stac;
    u64 u64Clac;
    u64 u64SmapOn;
    u64 u64Chunks;
    u64 u64PeakFrom;
    u64 u64PeakTo;
    u64 u64LastFrom;
    u64 u64LastTo;
    u64 u64Logs;
    u32 u32Areas;
    u32 u32Surf;
    u32 u32LeanOk;
    u32 u32LeanExpect;

    /* Snapshot soft counters (diagnostics only; no hard lock needed). */
    u64FromOk = g_stats.u64FromOk;
    u64FromFault = g_stats.u64FromFault;
    u64FromInval = g_stats.u64FromInval;
    u64ToOk = g_stats.u64ToOk;
    u64ToFault = g_stats.u64ToFault;
    u64ToInval = g_stats.u64ToInval;
    u64LoadOk = g_stats.u64LoadOk;
    u64LoadFault = g_stats.u64LoadFault;
    u64LoadInval = g_stats.u64LoadInval;
    u64StoreOk = g_stats.u64StoreOk;
    u64StoreFault = g_stats.u64StoreFault;
    u64BytesFrom = g_stats.u64BytesFrom;
    u64BytesTo = g_stats.u64BytesTo;
    u64RangeOkFail = g_stats.u64RangeOkFail;
    u64RangeMapFail = g_stats.u64RangeMapFail;
    u64Stac = g_stats.u64Stac;
    u64Clac = g_stats.u64Clac;
    u64SmapOn = g_fSmapOn ? 1ull : 0ull;
    u64Chunks = g_stats.u64Chunks;
    u64PeakFrom = g_u64SoftPeakFrom;
    u64PeakTo = g_u64SoftPeakTo;
    u64LastFrom = g_u64SoftLastFrom;
    u64LastTo = g_u64SoftLastTo;

    if (g_u64SoftInventoryLogs < ~0ull) {
        g_u64SoftInventoryLogs++;
    }
    u64Logs = g_u64SoftInventoryLogs;
    u32Areas = 0;
    u32Surf = USER_COPY_SOFT_SURF_CATALOG;
    u32LeanExpect = USER_COPY_SOFT_LEAN_CHECKS;

    /*
     * Honesty first: freestanding soft inventory is NOT product SEH / full SMAP.
     * Grep: user_copy: soft honesty
     */
    kprintf("user_copy: soft honesty not-product-SEH not-full-SMAP "
            "g_ptr=soft smap_window=page_chunk product_seh=OPEN "
            "full_smap=OPEN exception_port=OPEN ddi_mint=OPEN "
            "(soft inventory only; soft!=product; never hard-gates)\n");
    u32Areas++;

    /* Grep: user_copy: soft inventory */
    kprintf("user_copy: soft inventory "
            "from_ok=%llu from_fault=%llu from_inval=%llu "
            "to_ok=%llu to_fault=%llu to_inval=%llu "
            "load_ok=%llu load_fault=%llu load_inval=%llu "
            "store_ok=%llu store_fault=%llu "
            "bytes_from=%llu bytes_to=%llu "
            "peak_from=%llu peak_to=%llu last_from=%llu last_to=%llu "
            "range_ok_fail=%llu range_map_fail=%llu "
            "smap=%llu stac=%llu clac=%llu chunks=%llu logs=%llu "
            "(soft; not product SEH)\n",
            (unsigned long long)u64FromOk,
            (unsigned long long)u64FromFault,
            (unsigned long long)u64FromInval,
            (unsigned long long)u64ToOk,
            (unsigned long long)u64ToFault,
            (unsigned long long)u64ToInval,
            (unsigned long long)u64LoadOk,
            (unsigned long long)u64LoadFault,
            (unsigned long long)u64LoadInval,
            (unsigned long long)u64StoreOk,
            (unsigned long long)u64StoreFault,
            (unsigned long long)u64BytesFrom,
            (unsigned long long)u64BytesTo,
            (unsigned long long)u64PeakFrom,
            (unsigned long long)u64PeakTo,
            (unsigned long long)u64LastFrom,
            (unsigned long long)u64LastTo,
            (unsigned long long)u64RangeOkFail,
            (unsigned long long)u64RangeMapFail,
            (unsigned long long)u64SmapOn,
            (unsigned long long)u64Stac,
            (unsigned long long)u64Clac,
            (unsigned long long)u64Chunks,
            (unsigned long long)u64Logs);
    u32Areas++;

    /* Grep: user_copy: soft range */
    kprintf("user_copy: soft range ok_fail=%llu oversize=%llu "
            "below_base=%llu above_end=%llu overflow=%llu end_beyond=%llu "
            "base=0x%llx end=0x%llx max=%llu "
            "(range-ok axes; soft only)\n",
            (unsigned long long)u64RangeOkFail,
            (unsigned long long)g_u64SoftRangeOversize,
            (unsigned long long)g_u64SoftRangeBelowBase,
            (unsigned long long)g_u64SoftRangeAboveEnd,
            (unsigned long long)g_u64SoftRangeOverflow,
            (unsigned long long)g_u64SoftRangeEndBeyond,
            (unsigned long long)GJ_USER_VA_BASE,
            (unsigned long long)GJ_USER_VA_END,
            (unsigned long long)GJ_USER_COPY_MAX);
    u32Areas++;

    /* Grep: user_copy: soft map */
    kprintf("user_copy: soft map map_fail=%llu not_present=%llu "
            "not_user=%llu write_ro=%llu read_ok=%llu write_ok=%llu "
            "read_call=%llu write_call=%llu "
            "(P|U|W|COW soft; not product SEH)\n",
            (unsigned long long)u64RangeMapFail,
            (unsigned long long)g_u64SoftMapNotPresent,
            (unsigned long long)g_u64SoftMapNotUser,
            (unsigned long long)g_u64SoftMapWriteRo,
            (unsigned long long)g_u64SoftMapReadOk,
            (unsigned long long)g_u64SoftMapWriteOk,
            (unsigned long long)g_u64SoftMapReadCall,
            (unsigned long long)g_u64SoftMapWriteCall);
    u32Areas++;

    /* Grep: user_copy: soft chunk (C3 dual_page clamp) */
    kprintf("user_copy: soft chunk smap=%llu stac=%llu clac=%llu "
            "chunks=%llu peak_chunks=%llu last_chunks=%llu "
            "peak_pages=%llu last_pages=%llu "
            "dual_page=1 dual_bound=%llu dual_clamp=%llu "
            "geom_fail=%llu scalar_direct=%llu scalar_cross=%llu "
            "scalar_bytes=%llu "
            "(page-chunk SMAP window; C3 dual-side+scalar; soft only)\n",
            (unsigned long long)u64SmapOn,
            (unsigned long long)u64Stac,
            (unsigned long long)u64Clac,
            (unsigned long long)u64Chunks,
            (unsigned long long)g_u64SoftPeakChunks,
            (unsigned long long)g_u64SoftLastChunks,
            (unsigned long long)g_u64SoftPeakPages,
            (unsigned long long)g_u64SoftLastPages,
            (unsigned long long)g_u64SoftDualBound,
            (unsigned long long)g_u64SoftDualClamp,
            (unsigned long long)g_u64SoftChunkGeomFail,
            (unsigned long long)g_u64SoftScalarDirect,
            (unsigned long long)g_u64SoftScalarCross,
            (unsigned long long)g_u64SoftScalarBytes);
    u32Areas++;

    /* Grep: user_copy: soft zero */
    kprintf("user_copy: soft zero from=%llu to=%llu load=%llu "
            "(zero-len early return; soft only)\n",
            (unsigned long long)g_u64SoftZeroFrom,
            (unsigned long long)g_u64SoftZeroTo,
            (unsigned long long)g_u64SoftZeroLoad);
    u32Areas++;

    /*
     * Soft path honesty: ladder + explicit OPEN lamps.
     * Grep: user_copy: soft path
     */
    kprintf("user_copy: soft path "
            "range_ok->mapped_access->stac_dual_page_chunk->clac "
            "from|to|load_u32|store_u32 write_intent=W|COW "
            "smap_notify=1 dual_page=1 scalar_x=1 c3=1 "
            "product_seh=OPEN full_smap=OPEN "
            "exception_port=OPEN ddi_mint=OPEN "
            "(soft inventory; soft!=product)\n");
    u32Areas++;

    /*
     * DDI/UDX G-PTR residual honesty (lean; Soft!=product).
     * Grep: user_copy: soft ddi
     */
    kprintf("user_copy: soft ddi "
            "g_ptr=1 range_ok+copy_to for ddi_door GET/MAP_BAR "
            "map_note_min=%u dev_info_min=%u "
            "hosts=ddi_host_gj|rtl8168_udx|xhci_udx "
            "product_hosts=UDX dual_dod_ab=OPEN mint=OPEN "
            "g_ac1=1 never_fs_rtl_usb=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "(DDI residual; not product mint; not Dual DoD close)\n",
            (unsigned)USER_COPY_SOFT_DDI_MAP_NOTE_MIN,
            (unsigned)USER_COPY_SOFT_DDI_DEV_INFO_MIN);
    u32Areas++;

    /*
     * Lean residual self-check (once PASS|FAIL lamp; counters every emit).
     * Grep: user_copy: soft residual lean
     */
    u32LeanOk = user_copy_soft_lean_selfcheck();
    kprintf("user_copy: soft residual lean "
            "ok=%u/%u runs=%llu pass=%llu fail=%llu "
            "base=0x%llx end=0x%llx max=%llu smap=%llu "
            "c3=1 dual_page_chunk=1 scalar_x=1 soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 mint=OPEN "
            "g_ptr=1 ddi=1 "
            "(lean residual; never hard-gates; soft!=product)\n",
            (unsigned)u32LeanOk,
            (unsigned)u32LeanExpect,
            (unsigned long long)g_u64SoftLeanRuns,
            (unsigned long long)g_u64SoftLeanOk,
            (unsigned long long)g_u64SoftLeanFail,
            (unsigned long long)GJ_USER_VA_BASE,
            (unsigned long long)GJ_USER_VA_END,
            (unsigned long long)GJ_USER_COPY_MAX,
            (unsigned long long)u64SmapOn);
    if (g_fSoftLeanOnce == 0u) {
        g_fSoftLeanOnce = 1u;
        if (u32LeanOk == u32LeanExpect) {
            kprintf("user_copy: soft residual lean PASS "
                    "ok=%u/%u c3=1 dual_page_chunk=1 scalar_x=1 "
                    "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
                    "ddi_g_ptr=1 mint=OPEN\n",
                    (unsigned)u32LeanOk, (unsigned)u32LeanExpect);
        } else {
            kprintf("user_copy: soft residual lean FAIL "
                    "ok=%u/%u c3=1 soft_ne_product=1 (diagnostics only)\n",
                    (unsigned)u32LeanOk, (unsigned)u32LeanExpect);
        }
    }
    u32Areas++;

    /*
     * Grep: user_copy: soft deepen
     * areas tracks prior soft lines this emission (honesty..lean).
     * C3 residual: dual_page chunk + scalar_x (behavior), not stamp storm.
     */
    kprintf("user_copy: soft deepen areas=%u catalog=%u logs=%llu "
            "smap=%llu surf=0x%x lean_ok=%u "
            "c3=1 dual_page=1 scalar_x=1 "
            "(lean residual; not product SEH; soft!=product; DDI G-PTR)\n",
            (unsigned)u32Areas,
            (unsigned)USER_COPY_SOFT_AREAS,
            (unsigned long long)u64Logs,
            (unsigned long long)u64SmapOn,
            (unsigned)u32Surf,
            (unsigned)u32LeanOk);
    u32Areas++;

    (void)u32Areas;
}

/**
 * Note successful copy_from_user size for soft peak/last inventory.
 */
static void
user_copy_soft_note_from(size_t cb)
{
    u64 u64Cb = (u64)cb;

    g_u64SoftLastFrom = u64Cb;
    if (u64Cb > g_u64SoftPeakFrom) {
        g_u64SoftPeakFrom = u64Cb;
    }
}

/**
 * Note successful copy_to_user size for soft peak/last inventory.
 */
static void
user_copy_soft_note_to(size_t cb)
{
    u64 u64Cb = (u64)cb;

    g_u64SoftLastTo = u64Cb;
    if (u64Cb > g_u64SoftPeakTo) {
        g_u64SoftPeakTo = u64Cb;
    }
}

/**
 * Note page-chunk transfer soft peaks.
 * Pages estimate: chunk count ≈ pages touched under page-chunk path.
 */
static void
user_copy_soft_note_chunked(size_t cb, u64 u64Chunks)
{
    u64 u64Pages;

    g_u64SoftLastChunks = u64Chunks;
    if (u64Chunks > g_u64SoftPeakChunks) {
        g_u64SoftPeakChunks = u64Chunks;
    }
    /* Soft page span: each chunk ≤ one page; chunk count ≈ pages touched. */
    u64Pages = u64Chunks;
    if (cb == 0) {
        u64Pages = 0;
    }
    g_u64SoftLastPages = u64Pages;
    if (u64Pages > g_u64SoftPeakPages) {
        g_u64SoftPeakPages = u64Pages;
    }
}

void
user_access_smap_enabled(void)
{
    g_fSmapOn = 1;
    g_stats.u64SmapOn = 1;
    /* Grep: user_copy: soft (baseline inventory after SMAP notify) */
    user_copy_soft_inventory_log();
}

int
user_access_smap_is_enabled(void)
{
    return g_fSmapOn;
}

void
user_copy_stats_get(struct gj_user_copy_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    g_stats.u64SmapOn = g_fSmapOn ? 1ull : 0ull;
    *pOut = g_stats;
    /*
     * Emit soft inventory on stats read so bring-up smoke that
     * snapshots USER_COPY_STATS also greps copy_from/to lines.
     * greppable: user_copy: soft
     */
    user_copy_soft_inventory_log();
}

void
user_copy_stats_reset(void)
{
    u64 u64Smap = g_fSmapOn ? 1ull : 0ull;

    memset(&g_stats, 0, sizeof(g_stats));
    g_stats.u64SmapOn = u64Smap;
    g_u64SoftPeakFrom = 0;
    g_u64SoftPeakTo = 0;
    g_u64SoftLastFrom = 0;
    g_u64SoftLastTo = 0;
    g_u64SoftRangeOversize = 0;
    g_u64SoftRangeBelowBase = 0;
    g_u64SoftRangeAboveEnd = 0;
    g_u64SoftRangeOverflow = 0;
    g_u64SoftRangeEndBeyond = 0;
    g_u64SoftMapNotPresent = 0;
    g_u64SoftMapNotUser = 0;
    g_u64SoftMapWriteRo = 0;
    g_u64SoftMapReadOk = 0;
    g_u64SoftMapWriteOk = 0;
    g_u64SoftMapReadCall = 0;
    g_u64SoftMapWriteCall = 0;
    g_u64SoftZeroFrom = 0;
    g_u64SoftZeroTo = 0;
    g_u64SoftZeroLoad = 0;
    g_u64SoftPeakChunks = 0;
    g_u64SoftLastChunks = 0;
    g_u64SoftPeakPages = 0;
    g_u64SoftLastPages = 0;
    g_u64SoftDualBound = 0;
    g_u64SoftDualClamp = 0;
    g_u64SoftChunkGeomFail = 0;
    g_u64SoftScalarDirect = 0;
    g_u64SoftScalarCross = 0;
    g_u64SoftScalarBytes = 0;
    /* Preserve inventory log count across reset (emission lifetime). */
    /* Grep: user_copy: soft (zeroed inventory after reset) */
    user_copy_soft_inventory_log();
}

/*
 * Arm AC for user access when SMAP is live. Always paired with
 * user_access_end before return (no open STAC window across callers).
 */
static void
user_access_begin(void)
{
    if (g_fSmapOn) {
        g_stats.u64Stac++;
        __asm__ volatile("stac" ::: "memory");
    }
}

static void
user_access_end(void)
{
    if (g_fSmapOn) {
        g_stats.u64Clac++;
        __asm__ volatile("clac" ::: "memory");
    }
}

/**
 * Copy under a tight SMAP window: one STAC/CLAC per dual-side page-chunk
 * so AC is not held across multi-page spans longer than necessary (C3).
 *
 * Dual-side clamp (C3 residual deepen): chunk end is the earlier of src
 * page end and dst page end. Single-side src clamp left copy_to_user able
 * to touch two user pages under one STAC when kernel src was page-aligned.
 * Returns 0 on full transfer; -1 fail-closed if geometry yields zero chunk
 * with bytes remaining (caller maps to GJ_ERR_FAULT). Soft!=product.
 */
static int
copy_raw_chunked(void *pDst, const void *pSrc, size_t cb)
{
    u8 *pD = (u8 *)pDst;
    const u8 *pS = (const u8 *)pSrc;
    size_t cbLeft = cb;
    u64 u64Chunks = 0;

    while (cbLeft > 0) {
        size_t cbOffS;
        size_t cbOffD;
        size_t cbChunkS;
        size_t cbChunkD;
        size_t cbChunk;

        cbOffS = (size_t)((u64)(gj_vaddr_t)pS & (u64)(GJ_PAGE_SIZE - 1));
        cbOffD = (size_t)((u64)(gj_vaddr_t)pD & (u64)(GJ_PAGE_SIZE - 1));
        cbChunkS = (size_t)GJ_PAGE_SIZE - cbOffS;
        cbChunkD = (size_t)GJ_PAGE_SIZE - cbOffD;
        /* Soft: note when dual-side bound differs from single-side src. */
        if (cbChunkS != cbChunkD) {
            if (g_u64SoftDualBound < ~0ull) {
                g_u64SoftDualBound++;
            }
        }
        cbChunk = user_copy_chunk_len(pD, pS, cbLeft);
        if (cbChunk < cbChunkS && cbChunk < cbLeft) {
            /* Dst page end was tighter than src (C3 copy_to_user path). */
            if (g_u64SoftDualClamp < ~0ull) {
                g_u64SoftDualClamp++;
            }
        }
        if (cbChunk == 0) {
            /* Geometry fail closed: never report OK on partial/zero progress. */
            if (g_u64SoftChunkGeomFail < ~0ull) {
                g_u64SoftChunkGeomFail++;
            }
            user_copy_soft_note_chunked(cb - cbLeft, u64Chunks);
            return -1;
        }
        user_access_begin();
        memcpy(pD, pS, cbChunk);
        user_access_end();
        g_stats.u64Chunks++;
        u64Chunks++;
        pD += cbChunk;
        pS += cbChunk;
        cbLeft -= cbChunk;
    }
    user_copy_soft_note_chunked(cb, u64Chunks);
    return 0;
}

int
user_range_ok(u64 u64Va, u64 u64Cb)
{
    u64 u64End;

    if (u64Cb == 0) {
        return 1;
    }
    /* Soft cap: never accept a single span larger than the user window. */
    if (u64Cb > GJ_USER_COPY_MAX) {
        g_stats.u64RangeOkFail++;
        g_u64SoftRangeOversize++;
        return 0;
    }
    if (u64Va < GJ_USER_VA_BASE) {
        g_stats.u64RangeOkFail++;
        g_u64SoftRangeBelowBase++;
        return 0;
    }
    if (u64Va >= GJ_USER_VA_END) {
        g_stats.u64RangeOkFail++;
        g_u64SoftRangeAboveEnd++;
        return 0;
    }
    u64End = u64Va + u64Cb;
    if (u64End < u64Va) {
        g_stats.u64RangeOkFail++;
        g_u64SoftRangeOverflow++;
        return 0; /* overflow */
    }
    if (u64End > GJ_USER_VA_END) {
        g_stats.u64RangeOkFail++;
        g_u64SoftRangeEndBeyond++;
        return 0;
    }
    return 1;
}

/**
 * Present + USER (+ write intent) for every page covering [va, va+cb).
 * Supervisor leftovers in the user band must not pass (G-MAP / G-PTR).
 */
int
user_range_mapped_access(u64 u64Va, u64 u64Cb, u32 u32Access)
{
    u64 u64Page;
    u64 u64End;
    u64 u64Pte;
    int fWrite;

    if (!user_range_ok(u64Va, u64Cb)) {
        return 0;
    }
    if (u64Cb == 0) {
        return 1;
    }
    if (u32Access == 0) {
        u32Access = GJ_USER_ACCESS_READ;
    }
    fWrite = (u32Access & GJ_USER_ACCESS_WRITE) != 0 ? 1 : 0;
    if (fWrite != 0) {
        g_u64SoftMapWriteCall++;
    } else {
        g_u64SoftMapReadCall++;
    }

    u64End = u64Va + u64Cb;
    u64Page = u64Va & ~(u64)(GJ_PAGE_SIZE - 1);
    for (;;) {
        u64Pte = vmm_read_pte((gj_vaddr_t)u64Page);
        if ((u64Pte & GJ_USER_PTE_P) == 0) {
            g_stats.u64RangeMapFail++;
            g_u64SoftMapNotPresent++;
            return 0;
        }
        if ((u64Pte & GJ_USER_PTE_U) == 0) {
            g_stats.u64RangeMapFail++;
            g_u64SoftMapNotUser++;
            return 0;
        }
        /*
         * Write intent: require hardware W or soft COW leaf.
         * Pure RO without COW fails closed before STAC (G-PTR soft).
         */
        if (fWrite != 0) {
            if ((u64Pte & GJ_USER_PTE_W) == 0 &&
                (u64Pte & GJ_USER_PTE_COW) == 0) {
                g_stats.u64RangeMapFail++;
                g_u64SoftMapWriteRo++;
                return 0;
            }
        }
        if (u64Page + (u64)GJ_PAGE_SIZE >= u64End) {
            break;
        }
        u64Page += (u64)GJ_PAGE_SIZE;
    }
    if (fWrite != 0) {
        g_u64SoftMapWriteOk++;
    } else {
        g_u64SoftMapReadOk++;
    }
    return 1;
}

int
user_range_mapped(u64 u64Va, u64 u64Cb)
{
    return user_range_mapped_access(u64Va, u64Cb, GJ_USER_ACCESS_READ);
}

gj_status_t
copy_from_user(void *pKdst, u64 u64Usrc, size_t cb)
{
    if (pKdst == NULL) {
        g_stats.u64FromInval++;
        return GJ_ERR_INVAL;
    }
    if (cb == 0) {
        g_u64SoftZeroFrom++;
        return GJ_OK;
    }
    if (!user_range_mapped_access(u64Usrc, (u64)cb, GJ_USER_ACCESS_READ)) {
        g_stats.u64FromFault++;
        return GJ_ERR_FAULT;
    }
    if (copy_raw_chunked(pKdst, (const void *)(gj_vaddr_t)u64Usrc, cb) != 0) {
        g_stats.u64FromFault++;
        return GJ_ERR_FAULT;
    }
    g_stats.u64FromOk++;
    g_stats.u64BytesFrom += (u64)cb;
    user_copy_soft_note_from(cb);
    return GJ_OK;
}

gj_status_t
copy_to_user(u64 u64Udst, const void *pKsrc, size_t cb)
{
    if (pKsrc == NULL) {
        g_stats.u64ToInval++;
        return GJ_ERR_INVAL;
    }
    if (cb == 0) {
        g_u64SoftZeroTo++;
        return GJ_OK;
    }
    if (!user_range_mapped_access(u64Udst, (u64)cb, GJ_USER_ACCESS_WRITE)) {
        g_stats.u64ToFault++;
        return GJ_ERR_FAULT;
    }
    if (copy_raw_chunked((void *)(gj_vaddr_t)u64Udst, pKsrc, cb) != 0) {
        g_stats.u64ToFault++;
        return GJ_ERR_FAULT;
    }
    g_stats.u64ToOk++;
    g_stats.u64BytesTo += (u64)cb;
    user_copy_soft_note_to(cb);
    return GJ_OK;
}

gj_status_t
user_load_u32(u64 u64Uaddr, u32 *pOut)
{
    u32 u32Tmp;
    u32 iByte;

    if (pOut == NULL) {
        g_stats.u64LoadInval++;
        return GJ_ERR_INVAL;
    }
    if (!user_range_mapped_access(u64Uaddr, sizeof(u32), GJ_USER_ACCESS_READ)) {
        g_stats.u64LoadFault++;
        return GJ_ERR_FAULT;
    }
    /*
     * C3 scalar dual_page: if u32 spans two user pages, load byte-wise with
     * one STAC/CLAC per byte so AC never covers two pages in one window.
     * Aligned / single-page keeps the fast one-shot volatile load.
     * greppable: user_copy: soft scalar dual_page=1
     */
    if (user_copy_u32_page_cross(u64Uaddr) != 0) {
        u32Tmp = 0;
        if (g_u64SoftScalarCross < ~0ull) {
            g_u64SoftScalarCross++;
        }
        for (iByte = 0; iByte < (u32)sizeof(u32); iByte++) {
            u8 u8B;

            user_access_begin();
            u8B = *(volatile u8 *)(gj_vaddr_t)(u64Uaddr + (u64)iByte);
            user_access_end();
            g_stats.u64Chunks++;
            if (g_u64SoftScalarBytes < ~0ull) {
                g_u64SoftScalarBytes++;
            }
            /* Little-endian assemble (x86_64 product). */
            u32Tmp |= ((u32)u8B) << (iByte * 8u);
        }
    } else {
        if (g_u64SoftScalarDirect < ~0ull) {
            g_u64SoftScalarDirect++;
        }
        user_access_begin();
        u32Tmp = *(volatile u32 *)(gj_vaddr_t)u64Uaddr;
        user_access_end();
        g_stats.u64Chunks++;
    }
    *pOut = u32Tmp;
    g_stats.u64LoadOk++;
    g_stats.u64BytesFrom += (u64)sizeof(u32);
    return GJ_OK;
}

gj_status_t
user_store_u32(u64 u64Uaddr, u32 u32Val)
{
    u32 iByte;

    if (!user_range_mapped_access(u64Uaddr, sizeof(u32),
                                  GJ_USER_ACCESS_WRITE)) {
        g_stats.u64StoreFault++;
        return GJ_ERR_FAULT;
    }
    /*
     * C3 scalar dual_page: page-crossing store uses per-byte STAC/CLAC.
     * greppable: user_copy: soft scalar dual_page=1
     */
    if (user_copy_u32_page_cross(u64Uaddr) != 0) {
        if (g_u64SoftScalarCross < ~0ull) {
            g_u64SoftScalarCross++;
        }
        for (iByte = 0; iByte < (u32)sizeof(u32); iByte++) {
            u8 u8B;

            u8B = (u8)((u32Val >> (iByte * 8u)) & 0xffu);
            user_access_begin();
            *(volatile u8 *)(gj_vaddr_t)(u64Uaddr + (u64)iByte) = u8B;
            user_access_end();
            g_stats.u64Chunks++;
            if (g_u64SoftScalarBytes < ~0ull) {
                g_u64SoftScalarBytes++;
            }
        }
    } else {
        if (g_u64SoftScalarDirect < ~0ull) {
            g_u64SoftScalarDirect++;
        }
        user_access_begin();
        *(volatile u32 *)(gj_vaddr_t)u64Uaddr = u32Val;
        user_access_end();
        g_stats.u64Chunks++;
    }
    g_stats.u64StoreOk++;
    g_stats.u64BytesTo += (u64)sizeof(u32);
    return GJ_OK;
}
