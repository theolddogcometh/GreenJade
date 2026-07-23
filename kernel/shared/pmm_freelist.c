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

int
gj_pmm_core_selftest(void)
{
    u64 a;
    u64 b;
    unsigned c0;

    c0 = g_cFree;
    a = gj_pmm_core_alloc();
    if (a == 0) {
        return 0;
    }
    b = gj_pmm_core_alloc();
    gj_pmm_core_free(a);
    if (b != 0) {
        gj_pmm_core_free(b);
    }
    return (g_cFree == c0) ? 1 : 0;
}
