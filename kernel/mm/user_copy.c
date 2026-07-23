/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * User pointer validation + copy with STAC/CLAC when SMAP is enabled.
 * G-PTR-*: range must sit in product user window, be present, and U=1.
 * Soft deepen: write-intent (W|COW), page-chunk SMAP window, soft stats.
 *
 * Soft copy_from/to_user inventory (Wave 9 exclusive):
 *   - Cumulative from/to/load/store ok|fault|inval + byte totals
 *   - Soft peaks / last transfer sizes (diagnostics only; wrap OK)
 *   - SMAP STAC/CLAC + page-chunk counters
 *   greppable: "user_copy: soft …"
 *
 * Honesty: soft inventory only — not product SEH / full SMAP claim.
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

static int                      g_fSmapOn;
static struct gj_user_copy_stats g_stats;

/*
 * Soft inventory extras (Wave 9; file-local — not hard product gates).
 * greppable: user_copy: soft
 */
static u64 g_u64SoftPeakFrom;      /* max successful copy_from_user cb */
static u64 g_u64SoftPeakTo;        /* max successful copy_to_user cb */
static u64 g_u64SoftLastFrom;      /* last successful copy_from_user cb */
static u64 g_u64SoftLastTo;        /* last successful copy_to_user cb */
static u64 g_u64SoftInventoryLogs; /* soft_inventory_log emissions */

static void user_copy_soft_inventory_log(void);
static void user_copy_soft_note_from(size_t cb);
static void user_copy_soft_note_to(size_t cb);

/**
 * Greppable soft copy_from/to_user inventory (product / smoke).
 *   user_copy: soft from_ok=… from_fault=… from_inval=… to_ok=… to_fault=… to_inval=…
 *   user_copy: soft load_ok=… load_fault=… load_inval=… store_ok=… store_fault=…
 *   user_copy: soft bytes_from=… bytes_to=… peak_from=… peak_to=… last_from=… last_to=…
 *   user_copy: soft range_ok_fail=… range_map_fail=… smap=… stac=… clac=… chunks=…
 *   user_copy: soft base=0x… end=0x… max=… logs=…
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

    /*
     * Grep: user_copy: soft
     * Soft inventory lamp only — not product SEH / full SMAP complete.
     */
    kprintf("user_copy: soft from_ok=%llu from_fault=%llu from_inval=%llu "
            "to_ok=%llu to_fault=%llu to_inval=%llu\n",
            (unsigned long long)u64FromOk,
            (unsigned long long)u64FromFault,
            (unsigned long long)u64FromInval,
            (unsigned long long)u64ToOk,
            (unsigned long long)u64ToFault,
            (unsigned long long)u64ToInval);
    kprintf("user_copy: soft load_ok=%llu load_fault=%llu load_inval=%llu "
            "store_ok=%llu store_fault=%llu\n",
            (unsigned long long)u64LoadOk,
            (unsigned long long)u64LoadFault,
            (unsigned long long)u64LoadInval,
            (unsigned long long)u64StoreOk,
            (unsigned long long)u64StoreFault);
    kprintf("user_copy: soft bytes_from=%llu bytes_to=%llu peak_from=%llu "
            "peak_to=%llu last_from=%llu last_to=%llu\n",
            (unsigned long long)u64BytesFrom,
            (unsigned long long)u64BytesTo,
            (unsigned long long)u64PeakFrom,
            (unsigned long long)u64PeakTo,
            (unsigned long long)u64LastFrom,
            (unsigned long long)u64LastTo);
    kprintf("user_copy: soft range_ok_fail=%llu range_map_fail=%llu "
            "smap=%llu stac=%llu clac=%llu chunks=%llu\n",
            (unsigned long long)u64RangeOkFail,
            (unsigned long long)u64RangeMapFail,
            (unsigned long long)u64SmapOn,
            (unsigned long long)u64Stac,
            (unsigned long long)u64Clac,
            (unsigned long long)u64Chunks);
    kprintf("user_copy: soft base=0x%llx end=0x%llx max=%llu logs=%llu "
            "(soft inventory; not product SEH)\n",
            (unsigned long long)GJ_USER_VA_BASE,
            (unsigned long long)GJ_USER_VA_END,
            (unsigned long long)GJ_USER_COPY_MAX,
            (unsigned long long)u64Logs);
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
        pD += cbChunk;
        pS += cbChunk;
        cbLeft -= cbChunk;
    }
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
        return 0;
    }
    if (u64Va < GJ_USER_VA_BASE) {
        g_stats.u64RangeOkFail++;
        return 0;
    }
    if (u64Va >= GJ_USER_VA_END) {
        g_stats.u64RangeOkFail++;
        return 0;
    }
    u64End = u64Va + u64Cb;
    if (u64End < u64Va) {
        g_stats.u64RangeOkFail++;
        return 0; /* overflow */
    }
    if (u64End > GJ_USER_VA_END) {
        g_stats.u64RangeOkFail++;
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

    u64End = u64Va + u64Cb;
    u64Page = u64Va & ~(u64)(GJ_PAGE_SIZE - 1);
    for (;;) {
        u64Pte = vmm_read_pte((gj_vaddr_t)u64Page);
        if ((u64Pte & GJ_USER_PTE_P) == 0) {
            g_stats.u64RangeMapFail++;
            return 0;
        }
        if ((u64Pte & GJ_USER_PTE_U) == 0) {
            g_stats.u64RangeMapFail++;
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
                return 0;
            }
        }
        if (u64Page + (u64)GJ_PAGE_SIZE >= u64End) {
            break;
        }
        u64Page += (u64)GJ_PAGE_SIZE;
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
