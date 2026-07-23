/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * User pointer validation + copy with STAC/CLAC when SMAP is enabled.
 * G-PTR-*: range must sit in product user window, be present, and U=1.
 * Soft deepen: write-intent (W|COW), page-chunk SMAP window, soft stats.
 *
 * Soft copy_from/to_user inventory (Wave 9 base; Wave 15 exclusive deepen):
 *   - Cumulative from/to/load/store ok|fault|inval + byte totals
 *   - Soft peaks / last transfer sizes (diagnostics only; wrap OK)
 *   - SMAP STAC/CLAC + page-chunk counters
 *   - Range-ok / map fail reason axes (soft only)
 *   - Zero-len early returns + pages/chunk peak soft
 *   greppable: "user_copy: soft …"
 *
 * Wave 15 soft inventory deepen (prefix-stable; greppable: user_copy: soft):
 *   "user_copy: soft honesty …"   explicit non-claims (not SEH / full SMAP)
 *   "user_copy: soft inventory …" rollup + wave stamp
 *   "user_copy: soft from_ok=…"   bulk from/to terminal status (legacy)
 *   "user_copy: soft load_ok=…"   scalar load/store status (legacy)
 *   "user_copy: soft bytes_from=…" bytes + peak/last (legacy)
 *   "user_copy: soft range …"     range-ok fail reason axes
 *   "user_copy: soft map …"       map fail reason axes + write intent
 *   "user_copy: soft chunk …"     STAC/CLAC + chunk/page soft peaks
 *   "user_copy: soft zero …"      zero-length early-return soft tallies
 *   "user_copy: soft path …"      surface catalog + honesty open lamps
 *   "user_copy: soft stats …"     aggregate rollup
 *   "user_copy: soft deepen …"    wave=15 stamp + area count
 *   "user_copy: soft lamps …"     SMAP/STAC readiness lamps
 *   "user_copy: soft window …"    Wave 15 user VA / max copy geometry
 * Honesty: soft inventory only — not product SEH / full SMAP claim; not bar3.
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

/* Wave 15 soft inventory stamp (file-local; never product gate). */
#define USER_COPY_SOFT_WAVE 15u

/* Soft inventory greppable area count (honesty..lamps+window; deepen excluded). */
#define USER_COPY_SOFT_AREAS 12u

static int                      g_fSmapOn;
static struct gj_user_copy_stats g_stats;

/*
 * Soft inventory extras (Wave 9 + Wave 15; file-local — not hard product gates).
 * greppable: user_copy: soft
 */
static u64 g_u64SoftPeakFrom;      /* max successful copy_from_user cb */
static u64 g_u64SoftPeakTo;        /* max successful copy_to_user cb */
static u64 g_u64SoftLastFrom;      /* last successful copy_from_user cb */
static u64 g_u64SoftLastTo;        /* last successful copy_to_user cb */
static u64 g_u64SoftInventoryLogs; /* soft_inventory_log emissions */

/* Wave 15: range-ok fail reason axes (soft; wrap OK). */
static u64 g_u64SoftRangeOversize;
static u64 g_u64SoftRangeBelowBase;
static u64 g_u64SoftRangeAboveEnd;
static u64 g_u64SoftRangeOverflow;
static u64 g_u64SoftRangeEndBeyond;

/* Wave 15: map fail reason axes (soft; wrap OK). */
static u64 g_u64SoftMapNotPresent;
static u64 g_u64SoftMapNotUser;
static u64 g_u64SoftMapWriteRo;    /* write intent, !W and !COW */
static u64 g_u64SoftMapReadOk;     /* mapped_access read intent soft ok */
static u64 g_u64SoftMapWriteOk;    /* mapped_access write intent soft ok */
static u64 g_u64SoftMapReadCall;   /* mapped_access read-intent calls */
static u64 g_u64SoftMapWriteCall;  /* mapped_access write-intent calls */

/* Wave 15: zero-length early returns + chunk/page soft peaks. */
static u64 g_u64SoftZeroFrom;
static u64 g_u64SoftZeroTo;
static u64 g_u64SoftZeroLoad;      /* load/store never zero-cb; reserved */
static u64 g_u64SoftPeakChunks;    /* max chunks in one copy_raw_chunked */
static u64 g_u64SoftLastChunks;    /* last transfer chunk count */
static u64 g_u64SoftPeakPages;     /* max pages spanned (soft estimate) */
static u64 g_u64SoftLastPages;     /* last transfer pages spanned */

static void user_copy_soft_inventory_log(void);
static void user_copy_soft_note_from(size_t cb);
static void user_copy_soft_note_to(size_t cb);
static void user_copy_soft_note_chunked(size_t cb, u64 u64Chunks);

/**
 * Greppable soft copy_from/to_user inventory (product / smoke; Wave 15 deepen).
 * Prefix-stable markers (user_copy: soft …):
 *   user_copy: soft honesty    — explicit non-claims
 *   user_copy: soft inventory  — rollup + wave
 *   user_copy: soft from_ok=…  — bulk from/to (legacy line shape)
 *   user_copy: soft load_ok=…  — load/store (legacy)
 *   user_copy: soft bytes_from=… — bytes/peak/last (legacy)
 *   user_copy: soft range      — range-ok fail axes
 *   user_copy: soft map        — map fail / intent axes
 *   user_copy: soft chunk      — STAC/CLAC + chunk/page peaks
 *   user_copy: soft zero       — zero-len early returns
 *   user_copy: soft path       — surface catalog + open lamps
 *   user_copy: soft stats      — aggregate rollup
 *   user_copy: soft deepen     — wave=15 stamp + areas
 *   user_copy: soft lamps      — SMAP readiness lamps
 *
 * Never allocates; safe from SMAP notify / stats get/reset.
 * Honesty: soft inventory only — not product SEH / full SMAP; not bar3.
 * greppable: user_copy: soft
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

    /*
     * Honesty first: freestanding soft inventory is NOT product SEH / full SMAP.
     * Grep: user_copy: soft honesty
     */
    kprintf("user_copy: soft honesty not-product-SEH not-full-SMAP "
            "g_ptr=soft smap_window=page_chunk product_seh=OPEN "
            "full_smap=OPEN bar3=OPEN wave=%u "
            "(soft inventory only; never hard-gates)\n",
            (unsigned)USER_COPY_SOFT_WAVE);
    u32Areas++;

    /* Grep: user_copy: soft inventory */
    kprintf("user_copy: soft inventory from_ok=%llu to_ok=%llu "
            "load_ok=%llu store_ok=%llu bytes_from=%llu bytes_to=%llu "
            "range_ok_fail=%llu range_map_fail=%llu smap=%llu chunks=%llu "
            "logs=%llu wave=%u "
            "(soft; not product SEH; not bar3)\n",
            (unsigned long long)u64FromOk,
            (unsigned long long)u64ToOk,
            (unsigned long long)u64LoadOk,
            (unsigned long long)u64StoreOk,
            (unsigned long long)u64BytesFrom,
            (unsigned long long)u64BytesTo,
            (unsigned long long)u64RangeOkFail,
            (unsigned long long)u64RangeMapFail,
            (unsigned long long)u64SmapOn,
            (unsigned long long)u64Chunks,
            (unsigned long long)u64Logs,
            (unsigned)USER_COPY_SOFT_WAVE);
    u32Areas++;

    /*
     * Grep: user_copy: soft
     * Soft inventory lamp only — not product SEH / full SMAP complete.
     * Legacy multi-field lines kept prefix-stable for smoke greps.
     */
    kprintf("user_copy: soft from_ok=%llu from_fault=%llu from_inval=%llu "
            "to_ok=%llu to_fault=%llu to_inval=%llu\n",
            (unsigned long long)u64FromOk,
            (unsigned long long)u64FromFault,
            (unsigned long long)u64FromInval,
            (unsigned long long)u64ToOk,
            (unsigned long long)u64ToFault,
            (unsigned long long)u64ToInval);
    u32Areas++;
    kprintf("user_copy: soft load_ok=%llu load_fault=%llu load_inval=%llu "
            "store_ok=%llu store_fault=%llu\n",
            (unsigned long long)u64LoadOk,
            (unsigned long long)u64LoadFault,
            (unsigned long long)u64LoadInval,
            (unsigned long long)u64StoreOk,
            (unsigned long long)u64StoreFault);
    u32Areas++;
    kprintf("user_copy: soft bytes_from=%llu bytes_to=%llu peak_from=%llu "
            "peak_to=%llu last_from=%llu last_to=%llu\n",
            (unsigned long long)u64BytesFrom,
            (unsigned long long)u64BytesTo,
            (unsigned long long)u64PeakFrom,
            (unsigned long long)u64PeakTo,
            (unsigned long long)u64LastFrom,
            (unsigned long long)u64LastTo);
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

    /* Grep: user_copy: soft chunk */
    kprintf("user_copy: soft chunk smap=%llu stac=%llu clac=%llu "
            "chunks=%llu peak_chunks=%llu last_chunks=%llu "
            "peak_pages=%llu last_pages=%llu "
            "(page-chunk SMAP window; soft only)\n",
            (unsigned long long)u64SmapOn,
            (unsigned long long)u64Stac,
            (unsigned long long)u64Clac,
            (unsigned long long)u64Chunks,
            (unsigned long long)g_u64SoftPeakChunks,
            (unsigned long long)g_u64SoftLastChunks,
            (unsigned long long)g_u64SoftPeakPages,
            (unsigned long long)g_u64SoftLastPages);
    u32Areas++;

    /* Grep: user_copy: soft zero */
    kprintf("user_copy: soft zero from=%llu to=%llu load=%llu "
            "(zero-len early return; soft only)\n",
            (unsigned long long)g_u64SoftZeroFrom,
            (unsigned long long)g_u64SoftZeroTo,
            (unsigned long long)g_u64SoftZeroLoad);
    u32Areas++;

    /*
     * Soft path honesty: surface catalog + explicit non-claims.
     * Grep: user_copy: soft path
     */
    kprintf("user_copy: soft path "
            "range_ok→mapped_access→stac_chunk→clac "
            "from|to|load_u32|store_u32 write_intent=W|COW "
            "smap_notify=1 product_seh=OPEN full_smap=OPEN "
            "exception_port=OPEN bar3=OPEN wave=%u "
            "(soft inventory; not bar3)\n",
            (unsigned)USER_COPY_SOFT_WAVE);
    u32Areas++;

    /* Grep: user_copy: soft stats */
    kprintf("user_copy: soft stats from_ok=%llu from_fault=%llu "
            "from_inval=%llu to_ok=%llu to_fault=%llu to_inval=%llu "
            "load_ok=%llu store_ok=%llu bytes_from=%llu bytes_to=%llu "
            "range_ok_fail=%llu range_map_fail=%llu stac=%llu clac=%llu "
            "chunks=%llu smap=%llu logs=%llu wave=%u\n",
            (unsigned long long)u64FromOk,
            (unsigned long long)u64FromFault,
            (unsigned long long)u64FromInval,
            (unsigned long long)u64ToOk,
            (unsigned long long)u64ToFault,
            (unsigned long long)u64ToInval,
            (unsigned long long)u64LoadOk,
            (unsigned long long)u64StoreOk,
            (unsigned long long)u64BytesFrom,
            (unsigned long long)u64BytesTo,
            (unsigned long long)u64RangeOkFail,
            (unsigned long long)u64RangeMapFail,
            (unsigned long long)u64Stac,
            (unsigned long long)u64Clac,
            (unsigned long long)u64Chunks,
            (unsigned long long)u64SmapOn,
            (unsigned long long)u64Logs,
            (unsigned)USER_COPY_SOFT_WAVE);
    u32Areas++;

    /* Grep: user_copy: soft lamps */
    kprintf("user_copy: soft lamps smap_on=%llu stac_clac_armed=%llu "
            "peak_from=%llu peak_to=%llu peak_chunks=%llu peak_pages=%llu "
            "base=0x%llx end=0x%llx max=%llu wave=%u "
            "(soft readiness; not product gate)\n",
            (unsigned long long)u64SmapOn,
            (unsigned long long)u64SmapOn,
            (unsigned long long)u64PeakFrom,
            (unsigned long long)u64PeakTo,
            (unsigned long long)g_u64SoftPeakChunks,
            (unsigned long long)g_u64SoftPeakPages,
            (unsigned long long)GJ_USER_VA_BASE,
            (unsigned long long)GJ_USER_VA_END,
            (unsigned long long)GJ_USER_COPY_MAX,
            (unsigned)USER_COPY_SOFT_WAVE);
    u32Areas++;

    /* Grep: user_copy: soft window (Wave 15 geometry) */
    kprintf("user_copy: soft window base=0x%llx end=0x%llx max=%llu "
            "pte_p=1 pte_u=1 write=W|COW chunk=page_align "
            "smap=%llu peak_from=%llu peak_to=%llu wave=%u\n",
            (unsigned long long)GJ_USER_VA_BASE,
            (unsigned long long)GJ_USER_VA_END,
            (unsigned long long)GJ_USER_COPY_MAX,
            (unsigned long long)u64SmapOn,
            (unsigned long long)u64PeakFrom,
            (unsigned long long)u64PeakTo,
            (unsigned)USER_COPY_SOFT_WAVE);
    u32Areas++;

    /*
     * Legacy geometry line (Wave 9 shape) kept greppable.
     * Grep: user_copy: soft base=
     */
    kprintf("user_copy: soft base=0x%llx end=0x%llx max=%llu logs=%llu "
            "(soft inventory; not product SEH)\n",
            (unsigned long long)GJ_USER_VA_BASE,
            (unsigned long long)GJ_USER_VA_END,
            (unsigned long long)GJ_USER_COPY_MAX,
            (unsigned long long)u64Logs);

    /*
     * Grep: user_copy: soft deepen wave
     * areas tracks prior soft lines this emission (honesty..lamps).
     */
    kprintf("user_copy: soft deepen wave=%u areas=%u logs=%llu "
            "catalog=%u smap=%llu "
            "(Wave 15 exclusive; not product SEH; not bar3)\n",
            (unsigned)USER_COPY_SOFT_WAVE,
            (unsigned)u32Areas,
            (unsigned long long)u64Logs,
            (unsigned)USER_COPY_SOFT_AREAS,
            (unsigned long long)u64SmapOn);

    (void)USER_COPY_SOFT_AREAS;
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
 * Note page-chunk transfer soft peaks (Wave 15).
 * Pages estimate: ceil span of [src, src+cb) at page grain via chunk count.
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
 * Copy under a tight SMAP window: one STAC/CLAC per page-chunk so AC is
 * not held across multi-page spans longer than necessary (soft harden).
 */
static void
copy_raw_chunked(void *pDst, const void *pSrc, size_t cb)
{
    u8 *pD = (u8 *)pDst;
    const u8 *pS = (const u8 *)pSrc;
    size_t cbLeft = cb;
    u64 u64Chunks = 0;

    while (cbLeft > 0) {
        size_t cbOff = (size_t)((u64)(gj_vaddr_t)pS & (u64)(GJ_PAGE_SIZE - 1));
        size_t cbChunk = (size_t)GJ_PAGE_SIZE - cbOff;

        if (cbChunk > cbLeft) {
            cbChunk = cbLeft;
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
    copy_raw_chunked(pKdst, (const void *)(gj_vaddr_t)u64Usrc, cb);
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
    copy_raw_chunked((void *)(gj_vaddr_t)u64Udst, pKsrc, cb);
    g_stats.u64ToOk++;
    g_stats.u64BytesTo += (u64)cb;
    user_copy_soft_note_to(cb);
    return GJ_OK;
}

gj_status_t
user_load_u32(u64 u64Uaddr, u32 *pOut)
{
    u32 u32Tmp;

    if (pOut == NULL) {
        g_stats.u64LoadInval++;
        return GJ_ERR_INVAL;
    }
    if (!user_range_mapped_access(u64Uaddr, sizeof(u32), GJ_USER_ACCESS_READ)) {
        g_stats.u64LoadFault++;
        return GJ_ERR_FAULT;
    }
    user_access_begin();
    u32Tmp = *(volatile u32 *)(gj_vaddr_t)u64Uaddr;
    user_access_end();
    g_stats.u64Chunks++;
    *pOut = u32Tmp;
    g_stats.u64LoadOk++;
    g_stats.u64BytesFrom += (u64)sizeof(u32);
    return GJ_OK;
}

gj_status_t
user_store_u32(u64 u64Uaddr, u32 u32Val)
{
    if (!user_range_mapped_access(u64Uaddr, sizeof(u32),
                                  GJ_USER_ACCESS_WRITE)) {
        g_stats.u64StoreFault++;
        return GJ_ERR_FAULT;
    }
    user_access_begin();
    *(volatile u32 *)(gj_vaddr_t)u64Uaddr = u32Val;
    user_access_end();
    g_stats.u64Chunks++;
    g_stats.u64StoreOk++;
    g_stats.u64BytesTo += (u64)sizeof(u32);
    return GJ_OK;
}
