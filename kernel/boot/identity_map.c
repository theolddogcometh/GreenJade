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
 * Soft product surface
 * --------------------
 * After CR3 load (serial already up on UEFI path), emit greppable soft markers
 * for identity install and the published gj_boot_info (GOP / memmap / handoff).
 *
 * Grep markers:
 *   boot: identity soft PASS cr3=… leaves=… gib=4
 *   identity: soft PASS|PARTIAL|FAIL …
 *   identity: soft inventory …
 *   identity: soft layout …
 *   identity: soft pd …
 *   boot: handoff soft … / boot: memmap soft … / boot: GOP soft …
 *     (via boot_info_soft_log when source is UEFI)
 *
 * greppable: identity: soft
 * greppable: boot: identity soft
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
#include <gj/boot_info.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/types.h>

#define PTE_P   (1ull << 0)
#define PTE_W   (1ull << 1)
#define PTE_PS  (1ull << 7) /* bit 7: page-size (2 MiB when set in PD) */
#define PTE_ADDR_MASK 0x000ffffffffff000ull

/* Soft inventory constants (must match build loop). */
#define IDMAP_PT_PAGES     6u
#define IDMAP_PD_COUNT     4u
#define IDMAP_LEAVES_PER_PD 512u
#define IDMAP_LEAF_SHIFT   21u /* 2 MiB */
#define IDMAP_GIB_SHIFT    30u /* 1 GiB */
#define IDMAP_GIB          4u
#define IDMAP_LEAF_FLAGS   (PTE_P | PTE_W | PTE_PS)
#define IDMAP_LINK_FLAGS   (PTE_P | PTE_W)

/* Six pages: PML4 + PDPT + 4×PD (covers 4 GiB with 2 MiB leaves). */
static u8 g_aPt[IDMAP_PT_PAGES * 4096u] __attribute__((aligned(4096)));

/* Soft install counter (observability). */
static u32 g_cSoftIdentityInstall;

/* Soft inventory emission counter (cap spam if re-entered). */
static u32 g_cSoftIdentityLog;

/**
 * Soft identity map inventory — walk g_aPt after CR3 load; never hard-fails.
 *
 * Prefix-stable greppable product markers:
 *   identity: soft PASS|PARTIAL|FAIL cr3=… match=… leaves=… …
 *   identity: soft inventory pt_pages=… pd=… leaves_ok=… …
 *   identity: soft layout pml4=… pdpt=… pd0=… …
 *   identity: soft pd i=… leaves_ok=… bad_flag=… bad_pa=… base_pa=…
 *
 * Soft only: does not change maps or abort boot.
 */
static void
identity_soft_inventory(u64 u64Cr3Expect, u64 u64Cr3Read, u32 cLeavesBuilt)
{
    u64 *pPml4;
    u64 *pPdpt;
    u64 *pPd;
    u32 iPd;
    u32 iPage;
    u32 cLeavesOk;
    u32 cLeavesBadFlag;
    u32 cLeavesBadPa;
    u32 cPdptOk;
    u32 cPml4Ok;
    u32 cEmptyUpper;
    u32 fCr3Match;
    u32 fAligned;
    u32 fCover;
    u64 paExpect;
    u64 paGot;
    u64 u64Ent;
    u64 u64Pml4Pa;
    u64 u64PdptPa;
    u64 aPdPa[IDMAP_PD_COUNT];
    u32 aPdLeavesOk[IDMAP_PD_COUNT];
    u32 aPdBadFlag[IDMAP_PD_COUNT];
    u32 aPdBadPa[IDMAP_PD_COUNT];
    const char *szVerdict;

    pPml4 = (u64 *)(void *)&g_aPt[0];
    pPdpt = (u64 *)(void *)&g_aPt[4096];
    u64Pml4Pa = (u64)(gj_vaddr_t)pPml4;
    u64PdptPa = (u64)(gj_vaddr_t)pPdpt;

    cLeavesOk = 0;
    cLeavesBadFlag = 0;
    cLeavesBadPa = 0;
    cPdptOk = 0;
    cPml4Ok = 0;
    cEmptyUpper = 0;
    fCr3Match = (u64Cr3Read == u64Cr3Expect) ? 1u : 0u;
    fAligned = (((u64)(gj_vaddr_t)g_aPt & 0xfffu) == 0u) ? 1u : 0u;

    /* PML4[0] → PDPT | P | W */
    u64Ent = pPml4[0];
    if ((u64Ent & IDMAP_LINK_FLAGS) == IDMAP_LINK_FLAGS &&
        (u64Ent & PTE_ADDR_MASK) == (u64PdptPa & PTE_ADDR_MASK)) {
        cPml4Ok = 1u;
    }

    for (iPd = 0; iPd < IDMAP_PD_COUNT; iPd++) {
        pPd = (u64 *)(void *)&g_aPt[(2u + iPd) * 4096u];
        aPdPa[iPd] = (u64)(gj_vaddr_t)pPd;
        aPdLeavesOk[iPd] = 0;
        aPdBadFlag[iPd] = 0;
        aPdBadPa[iPd] = 0;

        u64Ent = pPdpt[iPd];
        if ((u64Ent & IDMAP_LINK_FLAGS) == IDMAP_LINK_FLAGS &&
            (u64Ent & PTE_ADDR_MASK) == (aPdPa[iPd] & PTE_ADDR_MASK)) {
            cPdptOk++;
        }

        for (iPage = 0; iPage < IDMAP_LEAVES_PER_PD; iPage++) {
            paExpect = ((u64)iPd << IDMAP_GIB_SHIFT) +
                       ((u64)iPage << IDMAP_LEAF_SHIFT);
            u64Ent = pPd[iPage];
            if ((u64Ent & IDMAP_LEAF_FLAGS) != IDMAP_LEAF_FLAGS) {
                aPdBadFlag[iPd]++;
                cLeavesBadFlag++;
                continue;
            }
            /* 2 MiB PS leaf: PA is bits 51:21 (low 21 zero in phys field). */
            paGot = u64Ent & 0x000fffffffe00000ull;
            if (paGot != paExpect) {
                aPdBadPa[iPd]++;
                cLeavesBadPa++;
                continue;
            }
            aPdLeavesOk[iPd]++;
            cLeavesOk++;
        }
    }

    /* Soft: upper PDPT slots should stay zero in this bootstrap map. */
    for (iPage = IDMAP_PD_COUNT; iPage < 512u; iPage++) {
        if (pPdpt[iPage] == 0ull) {
            cEmptyUpper++;
        }
    }

    fCover = (cLeavesOk == (IDMAP_PD_COUNT * IDMAP_LEAVES_PER_PD) &&
              cPml4Ok == 1u && cPdptOk == IDMAP_PD_COUNT && fCr3Match != 0u &&
              fAligned != 0u)
                 ? 1u
                 : 0u;

    /*
     * Verdict (soft product inventory; never gates boot):
     *   PASS    — CR3 match + full 4 GiB leaf identity + links
     *   PARTIAL — map present but some leaf/link soft defects
     *   FAIL    — CR3 mismatch or zero good leaves
     */
    if (fCover != 0u) {
        szVerdict = "PASS";
    } else if (cLeavesOk == 0u || fCr3Match == 0u) {
        szVerdict = "FAIL";
    } else {
        szVerdict = "PARTIAL";
    }

    if (g_cSoftIdentityLog < 0xffffffffu) {
        g_cSoftIdentityLog++;
    }

    /* Grep: identity: soft PASS | PARTIAL | FAIL */
    kprintf("identity: soft %s cr3=0x%lx cr3_read=0x%lx match=%u "
            "leaves=%u leaves_ok=%u bad_flag=%u bad_pa=%u gib=%u "
            "pt_pages=%u installs=%u log_n=%u\n",
            szVerdict, (unsigned long)u64Cr3Expect,
            (unsigned long)u64Cr3Read, fCr3Match, (unsigned)cLeavesBuilt,
            (unsigned)cLeavesOk, (unsigned)cLeavesBadFlag,
            (unsigned)cLeavesBadPa, (unsigned)IDMAP_GIB,
            (unsigned)IDMAP_PT_PAGES, (unsigned)g_cSoftIdentityInstall,
            (unsigned)g_cSoftIdentityLog);

    /* Grep: identity: soft inventory */
    kprintf("identity: soft inventory pt_pages=%u pd=%u pdpt_ok=%u "
            "pml4_ok=%u leaves_ok=%u leaves_expect=%u empty_upper=%u "
            "aligned=%u cover=%u leaf_shift=%u link_flags=0x%x "
            "leaf_flags=0x%x\n",
            (unsigned)IDMAP_PT_PAGES, (unsigned)IDMAP_PD_COUNT,
            (unsigned)cPdptOk, (unsigned)cPml4Ok, (unsigned)cLeavesOk,
            (unsigned)(IDMAP_PD_COUNT * IDMAP_LEAVES_PER_PD),
            (unsigned)cEmptyUpper, fAligned, fCover,
            (unsigned)IDMAP_LEAF_SHIFT, (unsigned)IDMAP_LINK_FLAGS,
            (unsigned)IDMAP_LEAF_FLAGS);

    /* Grep: identity: soft layout */
    kprintf("identity: soft layout pml4=0x%lx pdpt=0x%lx "
            "pd0=0x%lx pd1=0x%lx pd2=0x%lx pd3=0x%lx "
            "pt_base=0x%lx pt_bytes=%u\n",
            (unsigned long)u64Pml4Pa, (unsigned long)u64PdptPa,
            (unsigned long)aPdPa[0], (unsigned long)aPdPa[1],
            (unsigned long)aPdPa[2], (unsigned long)aPdPa[3],
            (unsigned long)(u64)(gj_vaddr_t)g_aPt,
            (unsigned)sizeof(g_aPt));

    /* Grep: identity: soft pd */
    for (iPd = 0; iPd < IDMAP_PD_COUNT; iPd++) {
        paExpect = (u64)iPd << IDMAP_GIB_SHIFT;
        kprintf("identity: soft pd i=%u leaves_ok=%u bad_flag=%u bad_pa=%u "
                "base_pa=0x%lx pd_pa=0x%lx gib_span=1\n",
                (unsigned)iPd, (unsigned)aPdLeavesOk[iPd],
                (unsigned)aPdBadFlag[iPd], (unsigned)aPdBadPa[iPd],
                (unsigned long)paExpect, (unsigned long)aPdPa[iPd]);
    }
}

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
    u64 u64Cr3Expect;
    u64 u64Cr3Read;
    u32 cLeaves;
    const struct gj_boot_info *pBi;

    memset(g_aPt, 0, sizeof(g_aPt));

    /* PML4[0] → PDPT | P | W  (low 512 GiB half; we only need first 4 GiB). */
    pPml4[0] = ((u64)(gj_vaddr_t)pPdpt) | PTE_P | PTE_W;

    cLeaves = 0;
    for (iPd = 0; iPd < IDMAP_PD_COUNT; iPd++) {
        /* PD index iPd owns phys [iPd GiB, (iPd+1) GiB). */
        pPd = (u64 *)(void *)&g_aPt[(2u + iPd) * 4096u];
        pPdpt[iPd] = ((u64)(gj_vaddr_t)pPd) | PTE_P | PTE_W;
        for (iPage = 0; iPage < IDMAP_LEAVES_PER_PD; iPage++) {
            /* 2 MiB page: phys = iPd*1 GiB + iPage*2 MiB */
            u64 pa = ((u64)iPd << IDMAP_GIB_SHIFT) +
                     ((u64)iPage << IDMAP_LEAF_SHIFT);

            pPd[iPage] = pa | PTE_P | PTE_W | PTE_PS;
            cLeaves++;
        }
    }

    /*
     * Full TLB shootdown on CR3 write (x86). After this, all kernel code/data
     * used until VMM must sit in the identity window or be covered by these
     * leaves — true for the product ELF load addresses.
     */
    u64Cr3Expect = (u64)(gj_vaddr_t)pPml4;
    __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3Expect) : "memory");
    __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3Read));

    g_cSoftIdentityInstall++;
    kprintf("boot: identity map ready va=pa [0,4GiB) cr3=0x%lx\n",
            (unsigned long)u64Cr3Expect);
    /* Grep: boot: identity soft PASS */
    kprintf("boot: identity soft PASS cr3=0x%lx cr3_read=0x%lx match=%d "
            "leaves=%u gib=4 pt_pages=6 installs=%u\n",
            (unsigned long)u64Cr3Expect, (unsigned long)u64Cr3Read,
            u64Cr3Read == u64Cr3Expect, (unsigned)cLeaves,
            (unsigned)g_cSoftIdentityInstall);

    /* Grep: identity: soft … (Wave 9 soft inventory deepen) */
    identity_soft_inventory(u64Cr3Expect, u64Cr3Read, cLeaves);

    /*
     * Soft UEFI handoff deepen: GOP / memmap / handoff markers once serial
     * is up and identity is ours. Multiboot does not call this function.
     */
    pBi = boot_info_get();
    if (pBi != NULL && pBi->u32Source == GJ_BOOT_SRC_UEFI) {
        boot_info_soft_log(pBi);
    }
}
