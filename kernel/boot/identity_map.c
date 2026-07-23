/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Kernel-owned identity map of the first 4 GiB (VA == PA) using 2 MiB pages.
 *
 * Why this exists (UEFI product path, P-BOOT-1)
 * ---------------------------------------------
 * After ExitBootServices the firmware page tables still map enough of RAM for
 * the EFI loader to jump into KERNEL.ELF, but those tables are not ours:
 *   - PT pages may be read-only or incomplete for later HHDM / VMM edits
 *   - firmware may not cover the full low 4 GiB the early kernel assumes
 *   - we must not leave CR3 pointing at reclaimable BootServices memory
 *
 * Multiboot path already builds this same layout in arch/x86_64/boot.S before
 * kmain. This file is the pure-C twin for kmain_uefi → boot_install_identity_4gib().
 *
 * Layout (must match boot.S)
 * --------------------------
 *   g_aPt layout, 6 × 4 KiB pages, 4 KiB-aligned:
 *     [0] PML4   — only slot 0 used → PDPT
 *     [1] PDPT   — slots 0..3 → PD0..PD3 (one GiB each)
 *     [2..5] PD0..PD3 — 512 × 2 MiB PS leaves each → 4 GiB total
 *
 * Leaf PTE flags: PRESENT | RW | PS (2 MiB). No NX / user / global bits —
 * early boot only; VMM installs product maps (HHDM, W^X, SMEP/SMAP) later.
 *
 * Limits (not product end-state)
 * ------------------------------
 *   - Covers only [0, 4 GiB). Machines with >4 GiB RAM still boot; PMM uses
 *     the firmware memory map for usable frames, then HHDM grows coverage
 *     (P-MEM-3 / P-MEM-5). This map is a bridge, not the 1 TiB bar.
 *   - g_aPt lives in kernel BSS and must remain identity-reachable while
 *     CR3 points here (linked in the low identity window of the product ELF).
 *   - Call once, early, after ExitBootServices and before VMM mutates tables.
 *
 * Pure C11 freestanding; dual MIT OR Apache-2.0. No assembly required.
 */
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/types.h>

#define PTE_P   (1ull << 0)
#define PTE_W   (1ull << 1)
#define PTE_PS  (1ull << 7) /* bit 7: page-size (2 MiB when set in PD) */

/* Six pages: PML4 + PDPT + 4×PD (covers 4 GiB with 2 MiB leaves). */
static u8 g_aPt[6u * 4096u] __attribute__((aligned(4096)));

/**
 * Build PML4→PDPT→4×PD covering phys [0, 4 GiB) at VA=PA and load CR3.
 *
 * Safe to call once early on UEFI after ExitBootServices (long mode already
 * on; only CR3 is replaced). No heap — uses g_aPt only.
 *
 * Multiboot: do not call; boot.S already installed an equivalent map.
 */
void
boot_install_identity_4gib(void)
{
    u64 *pPml4 = (u64 *)(void *)&g_aPt[0];
    u64 *pPdpt = (u64 *)(void *)&g_aPt[4096];
    u64 *pPd;
    u32 iPd;
    u32 iPage;

    memset(g_aPt, 0, sizeof(g_aPt));

    /* PML4[0] → PDPT | P | W  (low 512 GiB half; we only need first 4 GiB). */
    pPml4[0] = ((u64)(gj_vaddr_t)pPdpt) | PTE_P | PTE_W;

    for (iPd = 0; iPd < 4u; iPd++) {
        /* PD index iPd owns phys [iPd GiB, (iPd+1) GiB). */
        pPd = (u64 *)(void *)&g_aPt[(2u + iPd) * 4096u];
        pPdpt[iPd] = ((u64)(gj_vaddr_t)pPd) | PTE_P | PTE_W;
        for (iPage = 0; iPage < 512u; iPage++) {
            /* 2 MiB page: phys = iPd*1 GiB + iPage*2 MiB */
            u64 pa = ((u64)iPd << 30) + ((u64)iPage << 21);

            pPd[iPage] = pa | PTE_P | PTE_W | PTE_PS;
        }
    }

    /*
     * Full TLB shootdown on CR3 write (x86). After this, all kernel code/data
     * used until VMM must sit in the identity window or be covered by these
     * leaves — true for the product ELF load addresses.
     */
    __asm__ volatile ("mov %0, %%cr3" : : "r"((u64)(gj_vaddr_t)pPml4) : "memory");
    kprintf("boot: identity map ready va=pa [0,4GiB) cr3=0x%lx\n",
            (unsigned long)(gj_vaddr_t)pPml4);
}
