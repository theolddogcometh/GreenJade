/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * User pointer validation + copy with STAC/CLAC when SMAP is enabled.
 * G-PTR-*: range must sit in product user window, be present, and U=1.
 * Soft deepen: write-intent (W|COW), page-chunk SMAP window, soft stats.
 */
#include <gj/config.h>
#include <gj/error.h>
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

void
user_access_smap_enabled(void)
{
    g_fSmapOn = 1;
    g_stats.u64SmapOn = 1;
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
}

void
user_copy_stats_reset(void)
{
    u64 u64Smap = g_fSmapOn ? 1ull : 0ull;

    memset(&g_stats, 0, sizeof(g_stats));
    g_stats.u64SmapOn = u64Smap;
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
