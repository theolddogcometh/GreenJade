/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * User pointer validation + copy with STAC/CLAC when SMAP is enabled.
 * G-PTR-*: range must sit in product user window, be present, and U=1.
 */
#include <gj/config.h>
#include <gj/error.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

/* x86_64 PTE bits used for product user-map checks (match vmm.c). */
#define GJ_USER_PTE_P (1ull << 0)
#define GJ_USER_PTE_U (1ull << 2)

static int g_fSmapOn;

void
user_access_smap_enabled(void)
{
    g_fSmapOn = 1;
}

static void
stac(void)
{
    if (g_fSmapOn) {
        __asm__ volatile ("stac" ::: "memory");
    }
}

static void
clac(void)
{
    if (g_fSmapOn) {
        __asm__ volatile ("clac" ::: "memory");
    }
}

int
user_range_ok(u64 u64Va, u64 u64Cb)
{
    u64 u64End;

    if (u64Cb == 0) {
        return 1;
    }
    if (u64Va < GJ_USER_VA_BASE) {
        return 0;
    }
    u64End = u64Va + u64Cb;
    if (u64End < u64Va) {
        return 0; /* overflow */
    }
    if (u64End > GJ_USER_VA_END) {
        return 0;
    }
    return 1;
}

/**
 * Present + USER leaf for every page covering [u64Va, u64Va+u64Cb).
 * Supervisor leftovers in the user band must not pass (G-MAP / G-PTR).
 */
int
user_range_mapped(u64 u64Va, u64 u64Cb)
{
    u64 u64Page;
    u64 u64End;
    u64 u64Pte;

    if (!user_range_ok(u64Va, u64Cb)) {
        return 0;
    }
    if (u64Cb == 0) {
        return 1;
    }
    u64End = u64Va + u64Cb;
    u64Page = u64Va & ~(u64)(GJ_PAGE_SIZE - 1);
    for (;;) {
        u64Pte = vmm_read_pte((gj_vaddr_t)u64Page);
        if ((u64Pte & GJ_USER_PTE_P) == 0) {
            return 0;
        }
        if ((u64Pte & GJ_USER_PTE_U) == 0) {
            return 0;
        }
        if (u64Page + GJ_PAGE_SIZE >= u64End) {
            break;
        }
        u64Page += GJ_PAGE_SIZE;
    }
    return 1;
}

gj_status_t
copy_from_user(void *pKdst, u64 u64Usrc, size_t cb)
{
    if (pKdst == NULL) {
        return GJ_ERR_INVAL;
    }
    if (cb == 0) {
        return GJ_OK;
    }
    if (!user_range_mapped(u64Usrc, (u64)cb)) {
        return GJ_ERR_FAULT;
    }
    stac();
    memcpy(pKdst, (const void *)(gj_vaddr_t)u64Usrc, cb);
    clac();
    return GJ_OK;
}

gj_status_t
copy_to_user(u64 u64Udst, const void *pKsrc, size_t cb)
{
    if (pKsrc == NULL) {
        return GJ_ERR_INVAL;
    }
    if (cb == 0) {
        return GJ_OK;
    }
    if (!user_range_mapped(u64Udst, (u64)cb)) {
        return GJ_ERR_FAULT;
    }
    stac();
    memcpy((void *)(gj_vaddr_t)u64Udst, pKsrc, cb);
    clac();
    return GJ_OK;
}

gj_status_t
user_load_u32(u64 u64Uaddr, u32 *pOut)
{
    if (pOut == NULL) {
        return GJ_ERR_INVAL;
    }
    if (!user_range_mapped(u64Uaddr, sizeof(u32))) {
        return GJ_ERR_FAULT;
    }
    stac();
    *pOut = *(volatile u32 *)(gj_vaddr_t)u64Uaddr;
    clac();
    return GJ_OK;
}

gj_status_t
user_store_u32(u64 u64Uaddr, u32 u32Val)
{
    if (!user_range_mapped(u64Uaddr, sizeof(u32))) {
        return GJ_ERR_FAULT;
    }
    stac();
    *(volatile u32 *)(gj_vaddr_t)u64Uaddr = u32Val;
    clac();
    return GJ_OK;
}
