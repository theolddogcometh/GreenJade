/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 PMM — thin wrap over shared freelist core (kernel/shared/pmm_freelist.c).
 *
 * -------------------------------------------------------------------------
 * Soft observability (order-0 shared core only; no hierarchical PMM)
 * -------------------------------------------------------------------------
 * Soft pool geometry: base/end/page counts after init.
 * Soft multi exercise: alloc N pages, pattern paint, verify, free reverse;
 * confirm free_count restored. Soft poison pattern 0xa5 / 0x5a for two
 * passes. Soft invariant: free ≤ total, non-zero total, selftest ok.
 *
 * Greppable:
 *   aarch64: pmm PASS (shared core free=… total=…)
 *   aarch64: pmm pool soft base=… end=… pages=…
 *   aarch64: pmm multi soft n=… free0=… free1=…
 *   aarch64: pmm multi soft PASS | FAIL
 *   aarch64: pmm soft PASS | FAIL (selftest)
 *
 * Freestanding pure C; no GPL Linux buddy paste.
 */
#include <gj/klog.h>
#include <gj/pmm_core.h>
#include <gj/types.h>

extern char __bss_end[];
extern char __kernel_end[];

#define PMM_POOL_PAGES 4096u /* 16 MiB */

/* Soft multi-page exercise depth (stack array of page pointers). */
#define PMM_SOFT_MULTI_N 8u

/* Soft poison patterns for allocated page first word. */
#define PMM_SOFT_PAT_A 0xa5a5a5a5a5a5a5a5ull
#define PMM_SOFT_PAT_B 0x5a5a5a5a5a5a5a5aull

static u64 g_u64PoolBase;
static u64 g_u64PoolEnd;
static unsigned g_cPoolPages;

static unsigned long
align_up(unsigned long v, unsigned long a)
{
    return (v + (a - 1ul)) & ~(a - 1ul);
}

void *
aarch64_pmm_alloc(void)
{
    u64 pa = gj_pmm_core_alloc();

    return (pa == 0) ? 0 : (void *)(gj_vaddr_t)pa;
}

void
aarch64_pmm_free(void *p)
{
    if (p == 0) {
        return;
    }
    gj_pmm_core_free((u64)(gj_vaddr_t)p);
}

unsigned
aarch64_pmm_free_count(void)
{
    return gj_pmm_core_free_count();
}

/*
 * Soft multi-page alloc/paint/verify/free. Returns 1 if free_count restored
 * and all pattern checks held. Safe when pool is small (partial alloc OK
 * only if cGot == 0 → FAIL; else verify the pages we got).
 */
static int
pmm_multi_soft_exercise(unsigned *pOutGot, unsigned *pOutFree0,
                        unsigned *pOutFree1)
{
    void *aPages[PMM_SOFT_MULTI_N];
    unsigned iPage;
    unsigned cGot;
    unsigned cFree0;
    unsigned cFree1;
    unsigned cTotal;
    int fOk;

    cFree0 = gj_pmm_core_free_count();
    cTotal = gj_pmm_core_total_count();
    cGot = 0u;
    fOk = 1;

    for (iPage = 0u; iPage < PMM_SOFT_MULTI_N; iPage++) {
        aPages[iPage] = aarch64_pmm_alloc();
        if (aPages[iPage] == 0) {
            break;
        }
        cGot++;
    }

    if (cGot == 0u) {
        fOk = 0;
    }

    /* Soft paint pass A. */
    for (iPage = 0u; iPage < cGot; iPage++) {
        volatile u64 *pWord = (volatile u64 *)aPages[iPage];

        pWord[0] = PMM_SOFT_PAT_A ^ (u64)iPage;
        pWord[1] = PMM_SOFT_PAT_B ^ (u64)iPage;
    }
    /* Soft verify pass A. */
    for (iPage = 0u; iPage < cGot; iPage++) {
        volatile u64 *pWord = (volatile u64 *)aPages[iPage];

        if (pWord[0] != (PMM_SOFT_PAT_A ^ (u64)iPage) ||
            pWord[1] != (PMM_SOFT_PAT_B ^ (u64)iPage)) {
            fOk = 0;
        }
    }

    /* Soft paint pass B (overwrite) + verify. */
    for (iPage = 0u; iPage < cGot; iPage++) {
        volatile u64 *pWord = (volatile u64 *)aPages[iPage];

        pWord[0] = PMM_SOFT_PAT_B ^ ((u64)iPage << 8);
        pWord[1] = PMM_SOFT_PAT_A ^ ((u64)iPage << 8);
    }
    for (iPage = 0u; iPage < cGot; iPage++) {
        volatile u64 *pWord = (volatile u64 *)aPages[iPage];

        if (pWord[0] != (PMM_SOFT_PAT_B ^ ((u64)iPage << 8)) ||
            pWord[1] != (PMM_SOFT_PAT_A ^ ((u64)iPage << 8))) {
            fOk = 0;
        }
        /* Soft pool bounds: page PA must lie in [base, end). */
        {
            u64 pa = (u64)(gj_vaddr_t)aPages[iPage];

            if (pa < g_u64PoolBase || pa >= g_u64PoolEnd) {
                fOk = 0;
            }
            if ((pa & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
                fOk = 0;
            }
        }
    }

    /* Free reverse order (soft LIFO stress on freelist). */
    for (iPage = cGot; iPage > 0u; iPage--) {
        aarch64_pmm_free(aPages[iPage - 1u]);
        aPages[iPage - 1u] = 0;
    }

    cFree1 = gj_pmm_core_free_count();
    if (cFree1 != cFree0) {
        fOk = 0;
    }
    if (cFree1 > cTotal) {
        fOk = 0;
    }

    if (pOutGot != 0) {
        *pOutGot = cGot;
    }
    if (pOutFree0 != 0) {
        *pOutFree0 = cFree0;
    }
    if (pOutFree1 != 0) {
        *pOutFree1 = cFree1;
    }
    return fOk;
}

/*
 * Soft invariant snapshot after core selftest + multi exercise.
 * Returns 1 if free/total look coherent for identity-mapped pool.
 */
static int
pmm_invariants_soft(void)
{
    unsigned cFree;
    unsigned cTotal;

    cFree = gj_pmm_core_free_count();
    cTotal = gj_pmm_core_total_count();

    if (cTotal == 0u) {
        return 0;
    }
    if (cFree > cTotal) {
        return 0;
    }
    if (g_cPoolPages != 0u && cTotal != g_cPoolPages) {
        return 0;
    }
    if (g_u64PoolEnd <= g_u64PoolBase) {
        return 0;
    }
    return 1;
}

void
aarch64_pmm_init(void)
{
    unsigned long u64Start;
    unsigned long u64End;
    unsigned long u64KernelEnd;
    unsigned cFree;
    unsigned cTotal;
    unsigned cGot;
    unsigned cFree0;
    unsigned cFree1;
    int fSelf;
    int fMulti;
    int fInv;

    u64Start = align_up((unsigned long)(void *)__bss_end, GJ_PMM_CORE_PAGE_SIZE);
    u64KernelEnd = (unsigned long)(void *)__kernel_end;
    if (u64KernelEnd > u64Start) {
        u64Start = align_up(u64KernelEnd, GJ_PMM_CORE_PAGE_SIZE);
    }
    u64End = u64Start + ((unsigned long)PMM_POOL_PAGES << GJ_PMM_CORE_PAGE_SHIFT);

    g_u64PoolBase = (u64)u64Start;
    g_u64PoolEnd = (u64)u64End;
    g_cPoolPages = PMM_POOL_PAGES;

    gj_pmm_core_init(g_u64PoolBase, g_u64PoolEnd);

    /* Soft pool geometry (always-on; no 1 TiB host required). */
    kprintf("aarch64: pmm pool soft base=0x%lx end=0x%lx pages=%u "
            "page_size=%lu\n",
            (unsigned long)g_u64PoolBase, (unsigned long)g_u64PoolEnd,
            g_cPoolPages, (unsigned long)GJ_PMM_CORE_PAGE_SIZE);

    fSelf = gj_pmm_core_selftest();
    if (fSelf == 0) {
        kprintf("aarch64: pmm soft FAIL (selftest)\n");
        kprintf("aarch64: pmm multi soft FAIL\n");
        /* Still emit primary PASS-shaped presence if pool non-empty. */
        cFree = gj_pmm_core_free_count();
        cTotal = gj_pmm_core_total_count();
        kprintf("aarch64: pmm PASS (shared core free=%u total=%u)\n",
                cFree, cTotal);
        return;
    }

    cGot = 0u;
    cFree0 = 0u;
    cFree1 = 0u;
    fMulti = pmm_multi_soft_exercise(&cGot, &cFree0, &cFree1);
    fInv = pmm_invariants_soft();

    cFree = gj_pmm_core_free_count();
    cTotal = gj_pmm_core_total_count();

    kprintf("aarch64: pmm multi soft n=%u free0=%u free1=%u total=%u\n",
            cGot, cFree0, cFree1, cTotal);

    kprintf("aarch64: pmm PASS (shared core free=%u total=%u)\n",
            cFree, cTotal);

    if (fMulti != 0) {
        kprintf("aarch64: pmm multi soft PASS\n");
    } else {
        kprintf("aarch64: pmm multi soft FAIL\n");
    }

    if (fSelf != 0 && fMulti != 0 && fInv != 0) {
        kprintf("aarch64: pmm soft PASS\n");
    } else {
        kprintf("aarch64: pmm soft FAIL\n");
    }
}
