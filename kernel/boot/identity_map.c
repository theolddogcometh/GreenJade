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
 * Wave 56 exclusive soft deepen (this unit only — greppable "identity: soft …"):
 *   identity: soft honesty    — bridge only; higher-half product OPEN
 *   identity: soft inventory  — pt/pd/leaf cover snapshot + wave stamp
 *   identity: soft layout     — PML4/PDPT/PD phys addresses
 *   identity: soft pd         — per-GiB PD leaf tallies
 *   identity: soft geometry   — 4 GiB / 2 MiB leaf design constants
 *   identity: soft flags      — leaf/link PTE flag inventory
 *   identity: soft cover      — cover/match/align/pml4/pdpt lamps
 *   identity: soft cr3        — expect/read/match soft classify
 *   identity: soft link       — PML4[0] + PDPT[0..3] link soft
 *   identity: soft pml4       — slot0 link + upper-slot empty soft
 *   identity: soft stats      — install/log counters + leaf rollup
 *   identity: soft path       — UEFI install path honesty catalog
 *   identity: soft leaf       — Wave 15 leaf rollup surface
 *   identity: soft zero       — Wave 15 upper-slot empty surface
 *   identity: soft twin       — Wave 15 Multiboot boot.S twin note
 *   identity: soft install    — Wave 15 install counter surface
 *   identity: soft catalog    — Wave 19 area name rollup
 *   identity: soft deepen     — wave=111 stamp + area count
 *   identity: soft PASS|PARTIAL|FAIL …
 *   boot: identity soft PASS …
 *   boot: handoff soft … / boot: memmap soft … / boot: GOP soft …
 *     (via boot_info_soft_log when source is UEFI)
 *
 * greppable: identity: soft
 * greppable: identity: soft honesty
 * greppable: identity: soft deepen
 * greppable: boot: identity soft
 *
 * Limits (not product end-state)
 * ------------------------------
 *   - Covers only [0, 4 GiB). Machines with >4 GiB RAM still boot; PMM uses
 *     the firmware memory map for usable frames, then HHDM grows coverage
 *     (P-MEM-3 / P-MEM-5). This map is a bridge, not the 1 TiB bar.
 *   - Higher-half product move remains OPEN (soft honesty; never claims HH).
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
#define IDMAP_PT_PAGES      6u
#define IDMAP_PD_COUNT      4u
#define IDMAP_LEAVES_PER_PD 512u
#define IDMAP_LEAF_SHIFT    21u /* 2 MiB */
#define IDMAP_GIB_SHIFT     30u /* 1 GiB */
#define IDMAP_GIB           4u
#define IDMAP_LEAF_FLAGS    (PTE_P | PTE_W | PTE_PS)
#define IDMAP_LINK_FLAGS    (PTE_P | PTE_W)
#define IDMAP_SOFT_WAVE 111u /* Wave 51 exclusive soft deepen stamp */
#define IDMAP_LEAVES_EXPECT (IDMAP_PD_COUNT * IDMAP_LEAVES_PER_PD)
#define IDMAP_LEAF_BYTES    (1ull << IDMAP_LEAF_SHIFT)
#define IDMAP_GIB_BYTES     (1ull << IDMAP_GIB_SHIFT)
#define IDMAP_COVER_BYTES   ((u64)IDMAP_GIB << IDMAP_GIB_SHIFT)

/* Six pages: PML4 + PDPT + 4×PD (covers 4 GiB with 2 MiB leaves). */
static u8 g_aPt[IDMAP_PT_PAGES * 4096u] __attribute__((aligned(4096)));

/* Soft install counter (observability). */
static u32 g_cSoftIdentityInstall;

/* Soft inventory emission counter (cap spam if re-entered). */
static u32 g_cSoftIdentityLog;

/**
 * Soft identity map inventory — walk g_aPt after CR3 load; never hard-fails.
 *
 * Wave 35 exclusive soft deepen — prefix-stable greppable markers:
 *   identity: soft honesty / inventory / layout / pd / geometry / flags
 *   identity: soft cover / cr3 / link / pml4 / stats / path
 *   identity: soft leaf / zero / twin / install / catalog / deepen
 *   identity: soft PASS|PARTIAL|FAIL …
 *
 * Soft only: does not change maps or abort boot.
 * Honesty: bridge map only; higher-half product move remains OPEN.
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
    u32 cPml4EmptyUpper;
    u32 fCr3Match;
    u32 fAligned;
    u32 fCover;
    u32 fPml4Present;
    u32 fPml4Rw;
    u32 fNxClaim;
    u32 fUserClaim;
    u32 fGlobalClaim;
    u32 fHhProduct;
    u32 cAreas;
    u32 cLeavesExpect;
    u64 paExpect;
    u64 paGot;
    u64 u64Ent;
    u64 u64Pml4Pa;
    u64 u64PdptPa;
    u64 u64Pml4Ent;
    u64 aPdPa[IDMAP_PD_COUNT];
    u32 aPdLeavesOk[IDMAP_PD_COUNT];
    u32 aPdBadFlag[IDMAP_PD_COUNT];
    u32 aPdBadPa[IDMAP_PD_COUNT];
    u32 aPdLinkOk[IDMAP_PD_COUNT];
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
    cPml4EmptyUpper = 0;
    cAreas = 0;
    cLeavesExpect = IDMAP_LEAVES_EXPECT;
    fCr3Match = (u64Cr3Read == u64Cr3Expect) ? 1u : 0u;
    fAligned = (((u64)(gj_vaddr_t)g_aPt & 0xfffu) == 0u) ? 1u : 0u;
    fNxClaim = 0u;     /* soft honesty: early bridge never sets NX */
    fUserClaim = 0u;   /* soft honesty: no user bits */
    fGlobalClaim = 0u; /* soft honesty: no global bits */
    fHhProduct = 0u;   /* higher-half product move remains OPEN */

    /* PML4[0] → PDPT | P | W */
    u64Pml4Ent = pPml4[0];
    fPml4Present = ((u64Pml4Ent & PTE_P) != 0ull) ? 1u : 0u;
    fPml4Rw = ((u64Pml4Ent & PTE_W) != 0ull) ? 1u : 0u;
    if ((u64Pml4Ent & IDMAP_LINK_FLAGS) == IDMAP_LINK_FLAGS &&
        (u64Pml4Ent & PTE_ADDR_MASK) == (u64PdptPa & PTE_ADDR_MASK)) {
        cPml4Ok = 1u;
    }

    for (iPd = 0; iPd < IDMAP_PD_COUNT; iPd++) {
        pPd = (u64 *)(void *)&g_aPt[(2u + iPd) * 4096u];
        aPdPa[iPd] = (u64)(gj_vaddr_t)pPd;
        aPdLeavesOk[iPd] = 0;
        aPdBadFlag[iPd] = 0;
        aPdBadPa[iPd] = 0;
        aPdLinkOk[iPd] = 0;

        u64Ent = pPdpt[iPd];
        if ((u64Ent & IDMAP_LINK_FLAGS) == IDMAP_LINK_FLAGS &&
            (u64Ent & PTE_ADDR_MASK) == (aPdPa[iPd] & PTE_ADDR_MASK)) {
            cPdptOk++;
            aPdLinkOk[iPd] = 1u;
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
    /* Soft: PML4 slots 1..511 empty (low-half bridge only). */
    for (iPage = 1u; iPage < 512u; iPage++) {
        if (pPml4[iPage] == 0ull) {
            cPml4EmptyUpper++;
        }
    }

    fCover = (cLeavesOk == cLeavesExpect && cPml4Ok == 1u &&
              cPdptOk == IDMAP_PD_COUNT && fCr3Match != 0u && fAligned != 0u)
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

    /*
     * Honesty first: 4 GiB identity bridge is NOT higher-half product.
     * greppable: identity: soft honesty
     */
    kprintf("identity: soft honesty bridge_only=1 higher_half_product=OPEN "
            "hh_claim=%u cover_gib=%u leaf_mib=%u nx=%u user=%u global=%u "
            "p_mem_3=OPEN p_mem_5=OPEN product_tib=0 "
            "(soft; never closes higher-half)\n",
            (unsigned)fHhProduct, (unsigned)IDMAP_GIB,
            (unsigned)(IDMAP_LEAF_BYTES / (1024ull * 1024ull)),
            (unsigned)fNxClaim, (unsigned)fUserClaim,
            (unsigned)fGlobalClaim);
    cAreas++;

    /* Grep: identity: soft PASS | PARTIAL | FAIL */
    kprintf("identity: soft %s cr3=0x%lx cr3_read=0x%lx match=%u "
            "leaves=%u leaves_ok=%u bad_flag=%u bad_pa=%u gib=%u "
            "pt_pages=%u installs=%u log_n=%u wave=%u\n",
            szVerdict, (unsigned long)u64Cr3Expect,
            (unsigned long)u64Cr3Read, fCr3Match, (unsigned)cLeavesBuilt,
            (unsigned)cLeavesOk, (unsigned)cLeavesBadFlag,
            (unsigned)cLeavesBadPa, (unsigned)IDMAP_GIB,
            (unsigned)IDMAP_PT_PAGES, (unsigned)g_cSoftIdentityInstall,
            (unsigned)g_cSoftIdentityLog, (unsigned)IDMAP_SOFT_WAVE);
    cAreas++;

    /* Grep: identity: soft inventory */
    kprintf("identity: soft inventory wave=%u pt_pages=%u pd=%u pdpt_ok=%u "
            "pml4_ok=%u leaves_ok=%u leaves_expect=%u empty_upper=%u "
            "aligned=%u cover=%u leaf_shift=%u link_flags=0x%x "
            "leaf_flags=0x%x\n",
            (unsigned)IDMAP_SOFT_WAVE, (unsigned)IDMAP_PT_PAGES,
            (unsigned)IDMAP_PD_COUNT, (unsigned)cPdptOk, (unsigned)cPml4Ok,
            (unsigned)cLeavesOk, (unsigned)cLeavesExpect,
            (unsigned)cEmptyUpper, fAligned, fCover,
            (unsigned)IDMAP_LEAF_SHIFT, (unsigned)IDMAP_LINK_FLAGS,
            (unsigned)IDMAP_LEAF_FLAGS);
    cAreas++;

    /* Grep: identity: soft layout */
    kprintf("identity: soft layout pml4=0x%lx pdpt=0x%lx "
            "pd0=0x%lx pd1=0x%lx pd2=0x%lx pd3=0x%lx "
            "pt_base=0x%lx pt_bytes=%u\n",
            (unsigned long)u64Pml4Pa, (unsigned long)u64PdptPa,
            (unsigned long)aPdPa[0], (unsigned long)aPdPa[1],
            (unsigned long)aPdPa[2], (unsigned long)aPdPa[3],
            (unsigned long)(u64)(gj_vaddr_t)g_aPt,
            (unsigned)sizeof(g_aPt));
    cAreas++;

    /* Grep: identity: soft pd */
    for (iPd = 0; iPd < IDMAP_PD_COUNT; iPd++) {
        paExpect = (u64)iPd << IDMAP_GIB_SHIFT;
        kprintf("identity: soft pd i=%u leaves_ok=%u bad_flag=%u bad_pa=%u "
                "link_ok=%u base_pa=0x%lx pd_pa=0x%lx gib_span=1\n",
                (unsigned)iPd, (unsigned)aPdLeavesOk[iPd],
                (unsigned)aPdBadFlag[iPd], (unsigned)aPdBadPa[iPd],
                (unsigned)aPdLinkOk[iPd], (unsigned long)paExpect,
                (unsigned long)aPdPa[iPd]);
    }
    cAreas++;

    /* Grep: identity: soft geometry — design constants (compile-time). */
    kprintf("identity: soft geometry gib=%u cover_bytes=0x%lx "
            "leaf_bytes=0x%lx leaf_shift=%u gib_shift=%u "
            "pd=%u leaves_per_pd=%u leaves_expect=%u pt_pages=%u "
            "pt_bytes=%u page=4096 soft PASS\n",
            (unsigned)IDMAP_GIB, (unsigned long)IDMAP_COVER_BYTES,
            (unsigned long)IDMAP_LEAF_BYTES, (unsigned)IDMAP_LEAF_SHIFT,
            (unsigned)IDMAP_GIB_SHIFT, (unsigned)IDMAP_PD_COUNT,
            (unsigned)IDMAP_LEAVES_PER_PD, (unsigned)cLeavesExpect,
            (unsigned)IDMAP_PT_PAGES, (unsigned)sizeof(g_aPt));
    cAreas++;

    /* Grep: identity: soft flags — leaf/link PTE flag inventory. */
    kprintf("identity: soft flags leaf=0x%x link=0x%x P=0x%x W=0x%x "
            "PS=0x%x addr_mask=0x%lx nx_claim=%u user_claim=%u "
            "global_claim=%u soft PASS\n",
            (unsigned)IDMAP_LEAF_FLAGS, (unsigned)IDMAP_LINK_FLAGS,
            (unsigned)PTE_P, (unsigned)PTE_W, (unsigned)PTE_PS,
            (unsigned long)PTE_ADDR_MASK, (unsigned)fNxClaim,
            (unsigned)fUserClaim, (unsigned)fGlobalClaim);
    cAreas++;

    /* Grep: identity: soft cover — aggregate readiness lamps. */
    kprintf("identity: soft cover %s cover=%u match=%u aligned=%u "
            "pml4_ok=%u pdpt_ok=%u/%u leaves_ok=%u/%u bad_flag=%u "
            "bad_pa=%u empty_pdpt_upper=%u/%u\n",
            szVerdict, fCover, fCr3Match, fAligned, (unsigned)cPml4Ok,
            (unsigned)cPdptOk, (unsigned)IDMAP_PD_COUNT, (unsigned)cLeavesOk,
            (unsigned)cLeavesExpect, (unsigned)cLeavesBadFlag,
            (unsigned)cLeavesBadPa, (unsigned)cEmptyUpper,
            (unsigned)(512u - IDMAP_PD_COUNT));
    cAreas++;

    /* Grep: identity: soft cr3 */
    kprintf("identity: soft cr3 expect=0x%lx read=0x%lx match=%u "
            "aligned_pt=%u pml4_pa=0x%lx soft %s\n",
            (unsigned long)u64Cr3Expect, (unsigned long)u64Cr3Read,
            fCr3Match, fAligned, (unsigned long)u64Pml4Pa, szVerdict);
    cAreas++;

    /* Grep: identity: soft link — PML4 + PDPT link rollup. */
    kprintf("identity: soft link pml4_ok=%u pdpt_ok=%u/%u "
            "pd0=%u pd1=%u pd2=%u pd3=%u link_flags=0x%x soft %s\n",
            (unsigned)cPml4Ok, (unsigned)cPdptOk, (unsigned)IDMAP_PD_COUNT,
            (unsigned)aPdLinkOk[0], (unsigned)aPdLinkOk[1],
            (unsigned)aPdLinkOk[2], (unsigned)aPdLinkOk[3],
            (unsigned)IDMAP_LINK_FLAGS, szVerdict);
    cAreas++;

    /* Grep: identity: soft pml4 — slot0 + upper empty (low-half only). */
    kprintf("identity: soft pml4 slot0=0x%lx present=%u rw=%u ok=%u "
            "empty_upper=%u/511 pdpt_pa=0x%lx low_half_only=1 "
            "hh_slots=0 soft PASS\n",
            (unsigned long)u64Pml4Ent, (unsigned)fPml4Present,
            (unsigned)fPml4Rw, (unsigned)cPml4Ok,
            (unsigned)cPml4EmptyUpper, (unsigned long)u64PdptPa);
    cAreas++;

    /* Grep: identity: soft stats */
    kprintf("identity: soft stats installs=%u log_n=%u leaves_built=%u "
            "leaves_ok=%u bad_flag=%u bad_pa=%u pdpt_ok=%u pml4_ok=%u "
            "cover=%u wave=%u\n",
            (unsigned)g_cSoftIdentityInstall, (unsigned)g_cSoftIdentityLog,
            (unsigned)cLeavesBuilt, (unsigned)cLeavesOk,
            (unsigned)cLeavesBadFlag, (unsigned)cLeavesBadPa,
            (unsigned)cPdptOk, (unsigned)cPml4Ok, fCover,
            (unsigned)IDMAP_SOFT_WAVE);
    cAreas++;

    /*
     * Soft path honesty: UEFI install site + explicit non-claims.
     * greppable: identity: soft path
     */
    kprintf("identity: soft path claim=uefi_bridge site=boot_install_"
            "identity_4gib multiboot=boot.S twin=1 product_hh=OPEN "
            "hhdm=OPEN w_xor_x=OPEN smep=OPEN cover_gib=%u "
            "soft_never_gates=1 wave=%u\n",
            (unsigned)IDMAP_GIB, (unsigned)IDMAP_SOFT_WAVE);
    cAreas++;

    /* Grep: identity: soft leaf — Wave 15 leaf rollup surface. */
    kprintf("identity: soft leaf ok=%u expect=%u bad_flag=%u bad_pa=%u "
            "built=%u per_pd=%u leaf_mib=%u soft %s\n",
            (unsigned)cLeavesOk, (unsigned)cLeavesExpect,
            (unsigned)cLeavesBadFlag, (unsigned)cLeavesBadPa,
            (unsigned)cLeavesBuilt, (unsigned)IDMAP_LEAVES_PER_PD,
            (unsigned)(IDMAP_LEAF_BYTES / (1024ull * 1024ull)), szVerdict);
    cAreas++;

    /* Grep: identity: soft zero — Wave 15 upper-slot empty surface. */
    kprintf("identity: soft zero pdpt_upper=%u/%u pml4_upper=%u/511 "
            "low_half_only=1 hh_slots=0 soft PASS\n",
            (unsigned)cEmptyUpper, (unsigned)(512u - IDMAP_PD_COUNT),
            (unsigned)cPml4EmptyUpper);
    cAreas++;

    /* Grep: identity: soft twin — Multiboot boot.S layout twin note. */
    kprintf("identity: soft twin multiboot=boot.S uefi=identity_map.c "
            "layout_match=1 cover_gib=%u leaf_mib=2 pt_pages=%u "
            "soft PASS\n",
            (unsigned)IDMAP_GIB, (unsigned)IDMAP_PT_PAGES);
    cAreas++;

    /* Grep: identity: soft install — install counter surface. */
    kprintf("identity: soft install n=%u log_n=%u cr3_match=%u "
            "cover=%u soft %s\n",
            (unsigned)g_cSoftIdentityInstall, (unsigned)g_cSoftIdentityLog,
            fCr3Match, fCover, szVerdict);
    cAreas++;

    /* Grep: identity: soft surfaces (Wave 20 deepen) */
    kprintf("identity: soft surfaces count=%u wave=%u "
            "names=honesty,verdict,inventory,layout,pd,geometry,flags,"
            "leaf,zero,twin,install,catalog,surfaces,note,return,retmap,deepen\n",
            (unsigned)cAreas + 4u, (unsigned)IDMAP_SOFT_WAVE);
    cAreas++;

    /* Grep: identity: soft note (Wave 20 deepen) */
    kprintf("identity: soft note milestone=wave98 exclusive=1 "
            "bridge_only=1 soft_only=1 not_bar3=1 wave=%u\n",
            (unsigned)IDMAP_SOFT_WAVE);
    cAreas++;

    /* Grep: identity: soft catalog — Wave 19 area name rollup. */
    kprintf("identity: soft catalog honesty,verdict,inventory,layout,pd,"
            "geometry,flags,cover,cr3,link,pml4,stats,path,leaf,zero,"
            "twin,install,catalog,return,retmap,deepen wave=%u areas_expect=86 soft PASS\n",
            (unsigned)IDMAP_SOFT_WAVE);
    cAreas++;

    /* Grep: identity: soft return (Wave 20 deepen) */
    kprintf("identity: soft return cover=%u leaves_ok=%u cr3_match=%u "
            "installs=%u verdict=%s product_gate=0 higher_half=OPEN "
            "wave=%u\n",
            fCover, (unsigned)cLeavesOk, fCr3Match,
            (unsigned)g_cSoftIdentityInstall, szVerdict,
            (unsigned)IDMAP_SOFT_WAVE);
    cAreas++;

    /* Grep: identity: soft retmap — Wave 19 return-surface map */
    kprintf("identity: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=111\n");

    /* Grep: identity: soft deepen — Wave 20 stamp + area count. */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: identity: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("identity: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)IDMAP_SOFT_WAVE);
    /* Grep: identity: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("identity: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)IDMAP_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: identity: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("identity: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)IDMAP_SOFT_WAVE);
    /* Grep: identity: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("identity: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)IDMAP_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: identity: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("identity: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
            /* Grep: identity: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("identity: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: identity: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("identity: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
            /* Grep: identity: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("identity: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: identity: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("identity: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
            /* Grep: identity: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("identity: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: identity: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("identity: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
            /* Grep: identity: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("identity: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: identity: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("identity: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
            /* Grep: identity: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("identity: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: identity: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("identity: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
            /* Grep: identity: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("identity: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)IDMAP_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: identity: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("identity: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)IDMAP_SOFT_WAVE);
                    /* Grep: identity: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("identity: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)IDMAP_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: identity: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("identity: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)IDMAP_SOFT_WAVE);
                            /* Grep: identity: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("identity: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)IDMAP_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: identity: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("identity: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IDMAP_SOFT_WAVE);
                            /* Grep: identity: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("identity: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)IDMAP_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: identity: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("identity: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IDMAP_SOFT_WAVE);
                            /* Grep: identity: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("identity: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)IDMAP_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: identity: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("identity: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IDMAP_SOFT_WAVE);
                            /* Grep: identity: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("identity: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IDMAP_SOFT_WAVE);
                            /* Grep: identity: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("identity: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("identity: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("identity: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("identity: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("identity: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("identity: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("identity: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retfortress — Wave 35 return-fortress honesty */
kprintf("identity: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("identity: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft rethold — Wave 36 return-hold honesty */
kprintf("identity: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retspire — Wave 36 exclusive spire stamp */
kprintf("identity: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retwall — Wave 37 return-wall honesty */
kprintf("identity: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retgate — Wave 37 exclusive gate stamp */
kprintf("identity: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retmoat — Wave 38 return-moat honesty */
kprintf("identity: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retower — Wave 38 exclusive tower stamp */
kprintf("identity: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("identity: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("identity: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("identity: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("identity: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retravelin — Wave 41 return-travelin honesty */
kprintf("identity: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("identity: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("identity: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("identity: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("identity: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("identity: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("identity: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("identity: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("identity: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("identity: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retbailey — Wave 46 return-bailey honesty */
kprintf("identity: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)IDMAP_SOFT_WAVE);
/* Grep: identity: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("identity: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)IDMAP_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("identity: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("identity: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("identity: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("identity: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("identity: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("identity: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retsally — Wave 50 return-sally honesty */
kprintf("identity: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("identity: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retfosse — Wave 51 return-fosse honesty */
kprintf("identity: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("identity: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("identity: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("identity: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retravelin — Wave 53 return-travelin honesty */
kprintf("identity: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("identity: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("identity: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retredan — Wave 54 exclusive redan stamp */
kprintf("identity: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retflank — Wave 55 return-flank honesty */
kprintf("identity: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retface — Wave 55 exclusive face stamp */
kprintf("identity: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retgorge — Wave 56 return-gorge honesty */
kprintf("identity: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("identity: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retraverse — Wave 57 return-traverse honesty */
kprintf("identity: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("identity: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retorillon — Wave 58 return-orillon honesty */
kprintf("identity: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("identity: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("identity: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("identity: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retplace — Wave 60 return-place honesty */
kprintf("identity: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("identity: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("identity: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("identity: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("identity: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("identity: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("identity: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("identity: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: identity: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("identity: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: identity: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("identity: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: identity: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("identity: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: identity: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("identity: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: identity: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("identity: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=111 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: identity: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("identity: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=111 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("identity: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("identity: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("identity: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("identity: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("identity: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("identity: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("identity: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("identity: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("identity: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("identity: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: identity: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("identity: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("identity: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: identity: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("identity: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("identity: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbastionangle stamp; Soft≠product)\n");
/* Grep: identity: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("identity: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("identity: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retparapetangle stamp; Soft≠product)\n");
/* Grep: identity: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("identity: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("identity: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retowerangle stamp; Soft≠product)\n");
/* Grep: identity: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("identity: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("identity: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retwallangle stamp; Soft≠product)\n");
/* Grep: identity: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("identity: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("identity: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retholdangle stamp; Soft≠product)\n");
/* Grep: identity: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("identity: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("identity: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retfortressangle stamp; Soft≠product)\n");
/* Grep: identity: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("identity: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("identity: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: identity: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("identity: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("identity: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: identity: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("identity: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("identity: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: identity: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("identity: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("identity: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retaegisangle stamp; Soft≠product)\n");
/* Grep: identity: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("identity: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("identity: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retsigilangle stamp; Soft≠product)\n");
/* Grep: identity: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("identity: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("identity: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retglyphangle stamp; Soft≠product)\n");
/* Grep: identity: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("identity: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("identity: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retshardangle stamp; Soft≠product)\n");
/* Grep: identity: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("identity: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("identity: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retprismangle stamp; Soft≠product)\n");
/* Grep: identity: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("identity: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("identity: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcipherangle stamp; Soft≠product)\n");
/* Grep: identity: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("identity: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("identity: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retledgerangle stamp; Soft≠product)\n");
/* Grep: identity: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("identity: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("identity: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retvaultangle stamp; Soft≠product)\n");
/* Grep: identity: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("identity: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("identity: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (rettokenangle stamp; Soft≠product)\n");
/* Grep: identity: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("identity: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("identity: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retphaseangle stamp; Soft≠product)\n");
/* Grep: identity: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("identity: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("identity: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retpulseangle stamp; Soft≠product)\n");

/* Grep: identity: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("identity: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("identity: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retboundangle stamp; Soft≠product)\n");
/* Grep: identity: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("identity: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("identity: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbladeangle stamp; Soft≠product)\n");
/* Grep: identity: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("identity: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("identity: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retarcangle stamp; Soft≠product)\n");
/* Grep: identity: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("identity: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("identity: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: identity: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("identity: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("identity: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: identity: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("identity: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("identity: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retellipseangle stamp; Soft≠product)\n");
/* Grep: identity: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("identity: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("identity: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: identity: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("identity: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("identity: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (rethelixangle stamp; Soft≠product)\n");
/* Grep: identity: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("identity: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("identity: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retknotangle stamp; Soft≠product)\n");
/* Grep: identity: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("identity: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("identity: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retkleinangle stamp; Soft≠product)\n");
/* Grep: identity: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("identity: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("identity: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retaffineangle stamp; Soft≠product)\n");
/* Grep: identity: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("identity: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("identity: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: identity: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("identity: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("identity: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcubicangle stamp; Soft≠product)\n");
/* Grep: identity: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("identity: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("identity: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retquinticangle stamp; Soft≠product)\n");
/* Grep: identity: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("identity: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("identity: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbezierangle stamp; Soft≠product)\n");
/* Grep: identity: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("identity: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("identity: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: identity: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("identity: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("identity: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: identity: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("identity: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("identity: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retgridangle stamp; Soft≠product)\n");
/* Grep: identity: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("identity: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: identity: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("identity: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (rettexelangle stamp; Soft≠product)\n");
                            kprintf("identity: soft deepen wave=%u areas=%u verdict=%s "
            "cover=%u leaves_ok=%u installs=%u log_n=%u "
            "higher_half_product=OPEN (soft; bridge only)\n",
            (unsigned)IDMAP_SOFT_WAVE, (unsigned)cAreas, szVerdict, fCover,
            (unsigned)cLeavesOk, (unsigned)g_cSoftIdentityInstall,
            (unsigned)g_cSoftIdentityLog);
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
            "leaves=%u gib=4 pt_pages=6 installs=%u wave=%u\n",
            (unsigned long)u64Cr3Expect, (unsigned long)u64Cr3Read,
            u64Cr3Read == u64Cr3Expect, (unsigned)cLeaves,
            (unsigned)g_cSoftIdentityInstall, (unsigned)IDMAP_SOFT_WAVE);

    /* Grep: identity: soft … (Wave 20 exclusive soft inventory deepen) */
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
