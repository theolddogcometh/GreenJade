/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 PMM — thin wrap over shared freelist core (kernel/shared/pmm_freelist.c).
 * Greppable: aarch64: pmm PASS
 */
#include <gj/klog.h>
#include <gj/pmm_core.h>

extern char __bss_end[];
extern char __kernel_end[];

#define PMM_POOL_PAGES 4096u /* 16 MiB */

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

void
aarch64_pmm_init(void)
{
    unsigned long u64Start;
    unsigned long u64End;
    unsigned long k;

    u64Start = align_up((unsigned long)(void *)__bss_end, GJ_PMM_CORE_PAGE_SIZE);
    k = (unsigned long)(void *)__kernel_end;
    if (k > u64Start) {
        u64Start = align_up(k, GJ_PMM_CORE_PAGE_SIZE);
    }
    u64End = u64Start + ((unsigned long)PMM_POOL_PAGES << GJ_PMM_CORE_PAGE_SHIFT);

    gj_pmm_core_init(u64Start, u64End);
    if (gj_pmm_core_selftest() == 0) {
        kprintf("aarch64: pmm soft FAIL (selftest)\n");
        return;
    }
    kprintf("aarch64: pmm PASS (shared core free=%u total=%u)\n",
            gj_pmm_core_free_count(), gj_pmm_core_total_count());
}
