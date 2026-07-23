/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clear USER bit on kernel half; enable SMEP/SMAP (G-MAP-1..3, P-MEM-6).
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/smep.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

#define PTE_P   (1ull << 0)
#define PTE_U   (1ull << 2)
#define PTE_PS  (1ull << 7)
#define PTE_ADDR_MASK 0x000ffffffffff000ull

#define CR4_SMEP (1ull << 20)
#define CR4_SMAP (1ull << 21)

/**
 * Page-table walk VA for a physical table frame.
 * Prefer HHDM (shared kernel half); identity only for early low PA.
 */
static u64 *
phys_to_virt(u64 u64Pa)
{
    return (u64 *)hhdm_to_virt((gj_paddr_t)u64Pa);
}

static u64
read_cr3(void)
{
    u64 u64Cr3;

    __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));
    return u64Cr3;
}

static u64
read_cr4(void)
{
    u64 u64Cr4;

    __asm__ volatile ("mov %%cr4, %0" : "=r"(u64Cr4));
    return u64Cr4;
}

static void
write_cr4(u64 u64Cr4)
{
    __asm__ volatile ("mov %0, %%cr4" : : "r"(u64Cr4) : "memory");
}

void
cpu_enable_smep(void)
{
    u64 u64Cr4 = read_cr4();

    u64Cr4 |= CR4_SMEP;
    write_cr4(u64Cr4);
    kprintf("cpu: SMEP enabled (CR4=0x%lx)\n", (unsigned long)u64Cr4);
}

void
cpu_enable_smap(void)
{
    u64 u64Cr4 = read_cr4();

    u64Cr4 |= CR4_SMAP;
    write_cr4(u64Cr4);
    /* Default AC clear — kernel must STAC before user access */
    __asm__ volatile ("clac" ::: "memory");
    user_access_smap_enabled();
    kprintf("cpu: SMAP enabled (CR4=0x%lx); copy_* uses STAC/CLAC\n",
            (unsigned long)u64Cr4);
}

/*
 * Walk identity PD (boot 0..4GiB). Clear U on any present mapping whose
 * VA is outside the Linux personality user window (G-MAP-1, G-MAP-2).
 * Large pages: clear U on whole 2MiB if base VA is outside user band.
 * 4K: clear U if page VA < USER_BASE or >= USER_END.
 */
void
vmm_harden_kernel_maps(void)
{
    u64 *pPml4 = phys_to_virt(read_cr3() & PTE_ADDR_MASK);
    u64 *pPdpt;
    u64 *pPd;
    u64 *pPt;
    u32 i3, i2, i1;
    u64 u64Cleared = 0;

    if ((pPml4[0] & PTE_P) == 0) {
        kprintf("vmm: harden: no PML4[0]\n");
        return;
    }
    pPdpt = phys_to_virt(pPml4[0] & PTE_ADDR_MASK);

    for (i3 = 0; i3 < 4; i3++) {
        if ((pPdpt[i3] & PTE_P) == 0) {
            continue;
        }
        if ((pPdpt[i3] & PTE_PS) != 0) {
            continue; /* 1G — skip */
        }
        pPd = phys_to_virt(pPdpt[i3] & PTE_ADDR_MASK);
        for (i2 = 0; i2 < 512; i2++) {
            u64 u64Va2m = ((u64)i3 << 30) | ((u64)i2 << 21);

            if ((pPd[i2] & PTE_P) == 0) {
                continue;
            }
            if ((pPd[i2] & PTE_PS) != 0) {
                /* 2MiB page — clear U if wholly outside user window */
                if (u64Va2m + (1ull << 21) <= GJ_USER_VA_BASE ||
                    u64Va2m >= GJ_USER_VA_END) {
                    if (pPd[i2] & PTE_U) {
                        pPd[i2] &= ~PTE_U;
                        u64Cleared++;
                    }
                }
                continue;
            }
            pPt = phys_to_virt(pPd[i2] & PTE_ADDR_MASK);
            for (i1 = 0; i1 < 512; i1++) {
                u64 u64Va = u64Va2m | ((u64)i1 << 12);

                if ((pPt[i1] & PTE_P) == 0) {
                    continue;
                }
                if (u64Va >= GJ_USER_VA_BASE && u64Va < GJ_USER_VA_END) {
                    continue; /* leave product user pages */
                }
                if (pPt[i1] & PTE_U) {
                    pPt[i1] &= ~PTE_U;
                    u64Cleared++;
                }
            }
        }
    }
    /* Reload CR3 to flush TLB */
    {
        u64 u64Cr3 = read_cr3();

        __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
    }
    kprintf("vmm: harden cleared USER on %lu kernel PTE/PDE(s)\n",
            (unsigned long)u64Cleared);
}
