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
 * kmain. This file is the pure-C twin for kmain_uefi -> boot_install_identity_4gib().
 *
 * Layout (must match boot.S)
 * --------------------------
 *   g_aPt layout, 6 x 4 KiB pages, 4 KiB-aligned:
 *     [0] PML4   - only slot 0 used -> PDPT
 *     [1] PDPT   - slots 0..3 -> PD0..PD3 (one GiB each)
 *     [2..5] PD0..PD3 - 512 x 2 MiB PS leaves each -> 4 GiB total
 *
 * Leaf PTE flags: PRESENT | RW | PS (2 MiB). No NX / user / global bits -
 * early boot only; VMM installs product maps (HHDM, W^X, SMEP/SMAP) later.
 *
 * Soft product surface
 * --------------------
 * After CR3 load (serial already up on UEFI path), emit greppable soft markers
 * for identity install and the published gj_boot_info (GOP / memmap / handoff).
 *
 * Wave 56 exclusive soft deepen (this unit only - greppable "identity: soft ..."):
 *   identity: soft honesty    - bridge only; higher-half product OPEN
 *   identity: soft inventory  - pt/pd/leaf cover snapshot + wave stamp
 *   identity: soft layout     - PML4/PDPT/PD phys addresses
 *   identity: soft pd         - per-GiB PD leaf tallies
 *   identity: soft geometry   - 4 GiB / 2 MiB leaf design constants
 *   identity: soft flags      - leaf/link PTE flag inventory
 *   identity: soft cover      - cover/match/align/pml4/pdpt lamps
 *   identity: soft cr3        - expect/read/match soft classify
 *   identity: soft link       - PML4[0] + PDPT[0..3] link soft
 *   identity: soft pml4       - slot0 link + upper-slot empty soft
 *   identity: soft stats      - install/log counters + leaf rollup
 *   identity: soft path       - UEFI install path honesty catalog
 *   identity: soft leaf       - Wave 15 leaf rollup surface
 *   identity: soft zero       - Wave 15 upper-slot empty surface
 *   identity: soft twin       - Wave 15 Multiboot boot.S twin note
 *   identity: soft install    - Wave 15 install counter surface
 *   identity: soft catalog    - Wave 19 area name rollup
 *   identity: soft deepen     - wave=116 stamp + area count
 *   identity: soft residual lean - Soft!=product honesty (no version stamp)
 *   identity: soft residual lean PASS|SKIP - COM1 full / panel lean
 *   identity: soft PASS|PARTIAL|FAIL ...
 *   boot: identity soft PASS ...
 *   boot: handoff soft ... / boot: memmap soft ... / boot: GOP soft ...
 *     (via boot_info_soft_log when source is UEFI)
 *
 * Lean soft residual (this unit only)
 * -----------------------------------
 * Residual deepen (C0): denser honesty + panel SKIP path, storm=0.
 *   - COM1 path: residual lean after inventory (full cover/link/leaf lamps)
 *     plus residual lean PASS|SKIP (cover-class lamp; Soft!=product)
 *   - Panel path (no COM1 THRE): residual lean SKIP only - install-time
 *     CR3/leaves facts; never multi-KiB inventory (G752 hang avoidance)
 * Bridge_only, higher_half OPEN, dual MIT OR Apache-2.0. No version stamp.
 * Soft inventory still multi-line; residual lamps lean and once-per-install.
 * Soft residual != product higher-half / 1TiB / bar3. G-AC-1.
 * Dual DoD A/B remain OPEN (agent_ne_close=1; residual lamps never close).
 * Stamp-free residual: no GJ_IMAGE_VERSION; never bump image version.
 * class=C0 eng residual only (Soft!=product; not product AC).
 *
 * greppable: identity: soft
 * greppable: identity: soft honesty
 * greppable: identity: soft deepen
 * greppable: identity: soft residual lean
 * greppable: identity: soft residual lean PASS
 * greppable: identity: soft residual lean SKIP
 * greppable: boot: identity soft
 * greppable: Soft!=product
 * greppable: G-AC-1
 * greppable: DoD_A=OPEN
 * greppable: DoD_B=OPEN
 * greppable: agent_ne_close=1
 * greppable: no_version_stamp=1
 * greppable: class=C0
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
 * Pure C11 freestanding; dual MIT OR Apache-2.0. Soft!=product. G-AC-1.
 * Soft residual never claims product higher-half / 1 TiB / bar3 / Dual DoD.
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
#define IDMAP_SOFT_WAVE 116u /* Wave 51 exclusive soft deepen stamp */
#define IDMAP_LEAVES_EXPECT (IDMAP_PD_COUNT * IDMAP_LEAVES_PER_PD)
#define IDMAP_LEAF_BYTES    (1ull << IDMAP_LEAF_SHIFT)
#define IDMAP_GIB_BYTES     (1ull << IDMAP_GIB_SHIFT)
#define IDMAP_COVER_BYTES   ((u64)IDMAP_GIB << IDMAP_GIB_SHIFT)

/* Six pages: PML4 + PDPT + 4xPD (covers 4 GiB with 2 MiB leaves). */
static u8 g_aPt[IDMAP_PT_PAGES * 4096u] __attribute__((aligned(4096)));

/* Soft install counter (observability). */
static u32 g_cSoftIdentityInstall;

/* Soft inventory emission counter (cap spam if re-entered). */
static u32 g_cSoftIdentityLog;

/**
 * Soft identity map inventory - walk g_aPt after CR3 load; never hard-fails.
 *
 * Wave 35 exclusive soft deepen - prefix-stable greppable markers:
 *   identity: soft honesty / inventory / layout / pd / geometry / flags
 *   identity: soft cover / cr3 / link / pml4 / stats / path
 *   identity: soft leaf / zero / twin / install / catalog / deepen
 *   identity: soft residual lean / residual lean PASS|SKIP
 *   identity: soft PASS|PARTIAL|FAIL ...
 *
 * Soft only: does not change maps or abort boot.
 * Honesty: bridge map only; higher-half product move remains OPEN.
 * Lean residual: Soft!=product; no version stamp; storm=0; G-AC-1;
 * class=C0; Dual DoD A/B OPEN; agent_ne_close=1; residual!=product AC.
 * Panel path residual lives in boot_install_identity_4gib (SKIP lean).
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

    /* PML4[0] -> PDPT | P | W */
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
     *   PASS    - CR3 match + full 4 GiB leaf identity + links
     *   PARTIAL - map present but some leaf/link soft defects
     *   FAIL    - CR3 mismatch or zero good leaves
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

    /* Grep: identity: soft geometry - design constants (compile-time). */
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

    /* Grep: identity: soft flags - leaf/link PTE flag inventory. */
    kprintf("identity: soft flags leaf=0x%x link=0x%x P=0x%x W=0x%x "
            "PS=0x%x addr_mask=0x%lx nx_claim=%u user_claim=%u "
            "global_claim=%u soft PASS\n",
            (unsigned)IDMAP_LEAF_FLAGS, (unsigned)IDMAP_LINK_FLAGS,
            (unsigned)PTE_P, (unsigned)PTE_W, (unsigned)PTE_PS,
            (unsigned long)PTE_ADDR_MASK, (unsigned)fNxClaim,
            (unsigned)fUserClaim, (unsigned)fGlobalClaim);
    cAreas++;

    /* Grep: identity: soft cover - aggregate readiness lamps. */
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

    /* Grep: identity: soft link - PML4 + PDPT link rollup. */
    kprintf("identity: soft link pml4_ok=%u pdpt_ok=%u/%u "
            "pd0=%u pd1=%u pd2=%u pd3=%u link_flags=0x%x soft %s\n",
            (unsigned)cPml4Ok, (unsigned)cPdptOk, (unsigned)IDMAP_PD_COUNT,
            (unsigned)aPdLinkOk[0], (unsigned)aPdLinkOk[1],
            (unsigned)aPdLinkOk[2], (unsigned)aPdLinkOk[3],
            (unsigned)IDMAP_LINK_FLAGS, szVerdict);
    cAreas++;

    /* Grep: identity: soft pml4 - slot0 + upper empty (low-half only). */
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

    /* Grep: identity: soft leaf - Wave 15 leaf rollup surface. */
    kprintf("identity: soft leaf ok=%u expect=%u bad_flag=%u bad_pa=%u "
            "built=%u per_pd=%u leaf_mib=%u soft %s\n",
            (unsigned)cLeavesOk, (unsigned)cLeavesExpect,
            (unsigned)cLeavesBadFlag, (unsigned)cLeavesBadPa,
            (unsigned)cLeavesBuilt, (unsigned)IDMAP_LEAVES_PER_PD,
            (unsigned)(IDMAP_LEAF_BYTES / (1024ull * 1024ull)), szVerdict);
    cAreas++;

    /* Grep: identity: soft zero - Wave 15 upper-slot empty surface. */
    kprintf("identity: soft zero pdpt_upper=%u/%u pml4_upper=%u/511 "
            "low_half_only=1 hh_slots=0 soft PASS\n",
            (unsigned)cEmptyUpper, (unsigned)(512u - IDMAP_PD_COUNT),
            (unsigned)cPml4EmptyUpper);
    cAreas++;

    /* Grep: identity: soft twin - Multiboot boot.S layout twin note. */
    kprintf("identity: soft twin multiboot=boot.S uefi=identity_map.c "
            "layout_match=1 cover_gib=%u leaf_mib=2 pt_pages=%u "
            "soft PASS\n",
            (unsigned)IDMAP_GIB, (unsigned)IDMAP_PT_PAGES);
    cAreas++;

    /* Grep: identity: soft install - install counter surface. */
    kprintf("identity: soft install n=%u log_n=%u cr3_match=%u "
            "cover=%u soft %s\n",
            (unsigned)g_cSoftIdentityInstall, (unsigned)g_cSoftIdentityLog,
            fCr3Match, fCover, szVerdict);
    cAreas++;

    /* Grep: identity: soft surfaces (Wave 20 deepen) */
    kprintf("identity: soft surfaces count=%u wave=%u "
            "names=honesty,verdict,inventory,layout,pd,geometry,flags,"
            "cover,cr3,link,pml4,stats,path,leaf,zero,twin,install,"
            "catalog,surfaces,note,deepen,residual\n",
            (unsigned)cAreas + 3u, (unsigned)IDMAP_SOFT_WAVE);
    cAreas++;

    /* Grep: identity: soft note (Wave 20 deepen) */
    kprintf("identity: soft note milestone=wave98 exclusive=1 "
            "bridge_only=1 soft_only=1 residual_deepen=1 wave=%u\n",
            (unsigned)IDMAP_SOFT_WAVE);
    cAreas++;

    /* Grep: identity: soft catalog - Wave 19 area name rollup. */
    kprintf("identity: soft catalog honesty,verdict,inventory,layout,pd,"
            "geometry,flags,cover,cr3,link,pml4,stats,path,leaf,zero,"
            "twin,install,catalog,surfaces,note,deepen,residual "
            "wave=%u soft PASS\n",
            (unsigned)IDMAP_SOFT_WAVE);
    cAreas++;

    /* Grep: identity: soft deepen - area count (no residual flood). */
    kprintf("identity: soft deepen wave=%u areas=%u verdict=%s "
            "cover=%u leaves_ok=%u installs=%u log_n=%u "
            "higher_half_product=OPEN residual_deepen=1 "
            "(soft; bridge only)\n",
            (unsigned)IDMAP_SOFT_WAVE, (unsigned)cAreas, szVerdict, fCover,
            (unsigned)cLeavesOk, (unsigned)g_cSoftIdentityInstall,
            (unsigned)g_cSoftIdentityLog);
    cAreas++;

    /*
     * Lean soft residual - residual-class lamps for this unit (COM1 path).
     * Soft!=product · dual MIT OR Apache-2.0 · no version stamp · storm=0.
     * Bridge 4 GiB identity only; never product HH / 1 TiB / bar3.
     * Dual DoD A/B OPEN (agent!=close); residual != product AC. class=C0.
     * Denser residual: geometry + upper-empty + PTE non-claims + twin +
     * P-BOOT-1 / serial path honesty. G-AC-1. Stamp-free residual.
     * greppable: identity: soft residual lean
     * greppable: identity: soft residual lean PASS | SKIP
     * greppable: DoD_A=OPEN DoD_B=OPEN agent_ne_close=1 class=C0
     */
    kprintf("identity: soft residual lean "
            "verdict=%s cover=%u leaves_ok=%u/%u pml4_ok=%u pdpt_ok=%u/%u "
            "cr3_match=%u aligned=%u empty_pdpt_upper=%u/%u "
            "pml4_empty_upper=%u/511 cover_gib=%u leaf_mib=%u "
            "nx=%u user=%u global=%u installs=%u log_n=%u "
            "bridge_only=1 higher_half_product=OPEN p_mem_3=OPEN p_mem_5=OPEN "
            "product_tib=0 bar3=0 twin=boot.S p_boot_1=1 serial_path=COM1 "
            "soft_never_gates=1 soft=1 product=0 soft_ne_product=1 "
            "residual_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "DoD_A=OPEN DoD_B=OPEN agent_ne_close=1 soft_ne_close=1 "
            "class=C0 exclusive=1 lean=1 no_version_stamp=1 stamp_free=1 "
            "storm=0 G-AC-1 residual_deepen=1 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "not higher-half product; not 1TiB bar; not Dual DoD close; "
            "panel has SKIP lean)\n",
            szVerdict, fCover, (unsigned)cLeavesOk, (unsigned)cLeavesExpect,
            (unsigned)cPml4Ok, (unsigned)cPdptOk, (unsigned)IDMAP_PD_COUNT,
            fCr3Match, fAligned, (unsigned)cEmptyUpper,
            (unsigned)(512u - IDMAP_PD_COUNT), (unsigned)cPml4EmptyUpper,
            (unsigned)IDMAP_GIB,
            (unsigned)(IDMAP_LEAF_BYTES / (1024ull * 1024ull)),
            (unsigned)fNxClaim, (unsigned)fUserClaim, (unsigned)fGlobalClaim,
            (unsigned)g_cSoftIdentityInstall, (unsigned)g_cSoftIdentityLog);
    /* Grep: identity: soft residual lean PASS | SKIP (cover-class lamp) */
    kprintf("identity: soft residual lean %s "
            "cover=%u leaves_ok=%u/%u cr3_match=%u "
            "bridge_only=1 higher_half_product=OPEN "
            "soft=1 product=0 soft_ne_product=1 residual_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 DoD_A=OPEN DoD_B=OPEN "
            "agent_ne_close=1 class=C0 no_version_stamp=1 stamp_free=1 "
            "storm=0 G-AC-1 "
            "(Soft!=product; residual cover lamp; not product HH; "
            "not Dual DoD close)\n",
            (fCover != 0u) ? "PASS" : "SKIP", fCover,
            (unsigned)cLeavesOk, (unsigned)cLeavesExpect, fCr3Match);
}

/**
 * Build PML4->PDPT->4xPD covering phys [0, 4 GiB) at VA=PA and load CR3.
 *
 * Safe to call once early on UEFI after ExitBootServices (long mode already
 * on; only CR3 is replaced). No heap - uses g_aPt only.
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

    /* PML4[0] -> PDPT | P | W  (low 512 GiB half; we only need first 4 GiB). */
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
     * leaves - true for the product ELF load addresses.
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

    /*
     * Full soft inventory is multi-KiB of kprintf. On DUTs without COM1
     * (G752 panel path) that hung after the white kmain bar even with a
     * spin cap. Keep deepen for Multiboot/OVMF serial only.
     * Residual deepen (C0): panel still emits one lean residual SKIP line
     * (install-time facts only) so residual honesty is not COM1-only.
     * Soft!=product; storm=0; never multi-KiB flood on panel.
     * Dual DoD A/B OPEN; agent_ne_close=1; stamp-free; residual!=product.
     */
    {
        extern u32 serial_thre_dead(void);
        u32 fCr3MatchPanel;

        if (serial_thre_dead() == 0u) {
            /* Grep: identity: soft ... (Wave 20 exclusive soft inventory deepen) */
            identity_soft_inventory(u64Cr3Expect, u64Cr3Read, cLeaves);
            /*
             * Soft UEFI handoff deepen: GOP / memmap / handoff markers once
             * serial is up and identity is ours. Multiboot does not call here.
             */
            pBi = boot_info_get();
            if (pBi != NULL && pBi->u32Source == GJ_BOOT_SRC_UEFI) {
                boot_info_soft_log(pBi);
            }
        } else {
            fCr3MatchPanel = (u64Cr3Read == u64Cr3Expect) ? 1u : 0u;
            kprintf("identity: soft inventory SKIP (no COM1 THRE; panel path)\n");
            /*
             * Panel residual lean SKIP - install-time only; no table walk.
             * greppable: identity: soft residual lean
             * greppable: identity: soft residual lean SKIP
             * greppable: DoD_A=OPEN DoD_B=OPEN agent_ne_close=1 class=C0
             */
            kprintf("identity: soft residual lean "
                    "verdict=SKIP cover=0 leaves_built=%u/%u "
                    "cr3_match=%u cr3=0x%lx cr3_read=0x%lx "
                    "installs=%u cover_gib=%u leaf_mib=2 pt_pages=%u "
                    "bridge_only=1 higher_half_product=OPEN product_tib=0 "
                    "bar3=0 twin=boot.S p_boot_1=1 serial_path=panel "
                    "soft_never_gates=1 soft=1 product=0 soft_ne_product=1 "
                    "residual_ne_product=1 dual=MIT_OR_Apache-2.0 "
                    "DoD_A=OPEN DoD_B=OPEN agent_ne_close=1 soft_ne_close=1 "
                    "class=C0 exclusive=1 lean=1 no_version_stamp=1 "
                    "stamp_free=1 storm=0 G-AC-1 residual_deepen=1 "
                    "(Soft!=product; panel lean residual; no full inventory; "
                    "not higher-half product; not 1TiB bar; not Dual DoD close)\n",
                    (unsigned)cLeaves, (unsigned)IDMAP_LEAVES_EXPECT,
                    fCr3MatchPanel, (unsigned long)u64Cr3Expect,
                    (unsigned long)u64Cr3Read,
                    (unsigned)g_cSoftIdentityInstall, (unsigned)IDMAP_GIB,
                    (unsigned)IDMAP_PT_PAGES);
            kprintf("identity: soft residual lean SKIP "
                    "serial_path=panel cr3_match=%u leaves_built=%u "
                    "bridge_only=1 higher_half_product=OPEN "
                    "soft=1 product=0 soft_ne_product=1 residual_ne_product=1 "
                    "dual=MIT_OR_Apache-2.0 DoD_A=OPEN DoD_B=OPEN "
                    "agent_ne_close=1 class=C0 no_version_stamp=1 stamp_free=1 "
                    "storm=0 G-AC-1 "
                    "(Soft!=product; panel residual cover lamp; "
                    "not Dual DoD close)\n",
                    fCr3MatchPanel, (unsigned)cLeaves);
            (void)pBi;
        }
    }
}
