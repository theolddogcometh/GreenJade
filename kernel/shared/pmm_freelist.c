/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Shared freestanding order-0 freelist (GJ_ARCH_* product).
 * Identity-mapped PA pool only — no HHDM, no hierarchical orders.
 */
#include <gj/pmm_core.h>
#include <gj/string.h>

struct pmm_core_node {
    struct pmm_core_node *pNext;
};

static struct pmm_core_node *g_pFree;
static u64 g_u64Base;
static u64 g_u64Limit;
static unsigned g_cFree;
static unsigned g_cTotal;

void
gj_pmm_core_init(u64 u64Base, u64 u64Limit)
{
    u64 pa;

    g_pFree = 0;
    g_cFree = 0;
    g_cTotal = 0;
    g_u64Base = u64Base;
    g_u64Limit = u64Limit;

    if (u64Limit <= u64Base) {
        return;
    }
    for (pa = u64Base; pa + GJ_PMM_CORE_PAGE_SIZE <= u64Limit;
         pa += GJ_PMM_CORE_PAGE_SIZE) {
        struct pmm_core_node *n = (struct pmm_core_node *)(void *)(gj_vaddr_t)pa;

        n->pNext = g_pFree;
        g_pFree = n;
        g_cFree++;
        g_cTotal++;
    }
}

u64
gj_pmm_core_alloc(void)
{
    struct pmm_core_node *n = g_pFree;

    if (n == 0) {
        return 0;
    }
    g_pFree = n->pNext;
    if (g_cFree > 0u) {
        g_cFree--;
    }
    memset(n, 0, sizeof(*n));
    return (u64)(gj_vaddr_t)(void *)n;
}

void
gj_pmm_core_free(u64 u64Pa)
{
    struct pmm_core_node *n;

    if (u64Pa < g_u64Base || u64Pa >= g_u64Limit) {
        return;
    }
    if ((u64Pa & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
        return;
    }
    n = (struct pmm_core_node *)(void *)(gj_vaddr_t)u64Pa;
    n->pNext = g_pFree;
    g_pFree = n;
    g_cFree++;
}

unsigned
gj_pmm_core_free_count(void)
{
    return g_cFree;
}

unsigned
gj_pmm_core_total_count(void)
{
    return g_cTotal;
}

/*
 * Soft deepen: reject path, alignment/range, free-count steps, LIFO reuse,
 * payload survive (node header scrub only), restore pool counts.
 * Returns 1 on PASS, 0 on soft FAIL. Does not drain the whole pool.
 */
int
gj_pmm_core_selftest(void)
{
    u64 paA;
    u64 paB;
    u64 paC;
    unsigned cFree0;
    unsigned cTotal0;
    volatile u32 *pMark;

    cFree0 = g_cFree;
    cTotal0 = g_cTotal;

    if (cTotal0 == 0u || cFree0 == 0u) {
        return 0;
    }
    if (cFree0 > cTotal0) {
        return 0;
    }

    /* Reject paths must not touch free count. */
    gj_pmm_core_free(0);
    gj_pmm_core_free(1ul); /* unaligned */
    gj_pmm_core_free(g_u64Limit); /* OOB high (limit exclusive) */
    if (g_u64Base >= GJ_PMM_CORE_PAGE_SIZE) {
        gj_pmm_core_free(g_u64Base - GJ_PMM_CORE_PAGE_SIZE);
    }
    if (g_cFree != cFree0 || g_cTotal != cTotal0) {
        return 0;
    }

    paA = gj_pmm_core_alloc();
    if (paA == 0) {
        return 0;
    }
    if ((paA & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
        gj_pmm_core_free(paA);
        return 0;
    }
    if (paA < g_u64Base || paA >= g_u64Limit) {
        gj_pmm_core_free(paA);
        return 0;
    }
    if (g_cFree != cFree0 - 1u || g_cTotal != cTotal0) {
        gj_pmm_core_free(paA);
        return 0;
    }

    paB = gj_pmm_core_alloc();
    if (paB == 0) {
        /* Single-page pool: free A and accept restore. */
        gj_pmm_core_free(paA);
        return (g_cFree == cFree0 && g_cTotal == cTotal0) ? 1 : 0;
    }
    if (paA == paB) {
        gj_pmm_core_free(paA);
        return 0;
    }
    if ((paB & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul ||
        paB < g_u64Base || paB >= g_u64Limit) {
        gj_pmm_core_free(paA);
        gj_pmm_core_free(paB);
        return 0;
    }
    if (g_cFree != cFree0 - 2u) {
        gj_pmm_core_free(paA);
        gj_pmm_core_free(paB);
        return 0;
    }

    /*
     * LIFO: free pushes front; next alloc must return paB.
     * Payload past node header survives free+alloc (only sizeof(node) scrubbed).
     */
    pMark = (volatile u32 *)(void *)((gj_vaddr_t)paB + 64ul);
    *pMark = 0xC0FFEEU;
    gj_pmm_core_free(paB);
    if (g_cFree != cFree0 - 1u) {
        gj_pmm_core_free(paA);
        return 0;
    }
    paC = gj_pmm_core_alloc();
    if (paC != paB) {
        if (paC != 0) {
            gj_pmm_core_free(paC);
        }
        gj_pmm_core_free(paA);
        return 0;
    }
    if (*pMark != 0xC0FFEEU) {
        gj_pmm_core_free(paA);
        gj_pmm_core_free(paC);
        return 0;
    }

    gj_pmm_core_free(paA);
    gj_pmm_core_free(paC);
    if (g_cFree != cFree0 || g_cTotal != cTotal0) {
        return 0;
    }
    return 1;
}
