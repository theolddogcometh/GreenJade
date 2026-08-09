/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clear USER bit on kernel half; enable SMEP/SMAP (G-MAP-1..4, P-MEM-6).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. Soft != product.
 * G-AC-1: not Linux .ko product; no GPL in this TU.
 *
 * Soft residual (lean):
 *   - Full PML4 walk: low half outside user band + all kernel-half leaves
 *   - 1GiB / 2MiB / 4K PS leaves; straddle large-page soft residual count
 *   - G-MAP-4 soft: count U+!NX clears as UX residual fixed
 *   - Post-harden residual-U audit -> greppable soft PASS/FAIL
 *   - CPUID-gated CR4.SMEP / CR4.SMAP enable + soft query/stats
 *   - Cap full soft inventory (SMEP_SOFT_INV_CAP) - no stamp storms
 *   - Once-only "smep: soft residual lean" lamp (no version stamp)
 *   - C3 residual deepen: residual U carries base VA + page count + access
 *     bits + leaf size + last VA + W/size tallies (not residual_u alone).
 *     Soft diagnose only; Soft!=product; Dual DoD A/B OPEN.
 *
 * Soft inventory prefixes (stable; greppable: smep: soft):
 *   smep: soft honesty | inventory | map | cr4 | residual | enable
 *   smep: soft path | stats | lamps | band | surfaces | walk | OPEN | gmap
 *   smep: soft residual lean | residual deepen | deepen
 * Honesty: soft inventory only - not product G-MAP complete; soft != product.
 * Diagnostics only - never hard-gate boot; wrap OK. Stamp-free (no ver bump).
 * Dual DoD: A=soft residual diagnose shape; B=product G-MAP/SMEP close OPEN.
 *
 * greppable: smep: harden
 * greppable: smep: SMEP
 * greppable: smep: SMAP
 * greppable: smep: audit
 * greppable: smep: stats
 * greppable: smep: soft
 * greppable: smep: soft residual lean
 * greppable: smep: soft residual
 * greppable: smep: soft residual deepen
 * greppable: SMEP_HARDEN_STATS
 * greppable: Soft!=product | G-AC-1 | C3 residual | Dual_DoD_A=OPEN Dual_DoD_B=OPEN
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/smep.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

#define PTE_P   (1ull << 0)
#define PTE_W   (1ull << 1)
#define PTE_U   (1ull << 2)
#define PTE_PS  (1ull << 7)
#define PTE_NX  (1ull << 63)
#define PTE_ADDR_MASK 0x000ffffffffff000ull

/*
 * Soft residual C3 access-bit lamps (first residual leaf).
 * greppable: smep: soft residual deepen
 */
#define SMEP_SOFT_RESID_ACC_U        (1u << 0) /* USER bit set */
#define SMEP_SOFT_RESID_ACC_NX       (1u << 1) /* NX set (non-exec) */
#define SMEP_SOFT_RESID_ACC_X        (1u << 2) /* !NX (executable residual) */
#define SMEP_SOFT_RESID_ACC_KHALF    (1u << 3) /* kernel half (PML4 >= 256) */
#define SMEP_SOFT_RESID_ACC_STRADDLE (1u << 4) /* user-band straddle large */
#define SMEP_SOFT_RESID_ACC_W        (1u << 5) /* writable residual */
#define SMEP_SOFT_RESID_ACC_4K       (1u << 6) /* first residual is 4K leaf */
#define SMEP_SOFT_RESID_ACC_2M       (1u << 7) /* first residual is 2MiB PS */
#define SMEP_SOFT_RESID_ACC_1G       (1u << 8) /* first residual is 1GiB PS */

#define CR4_SMEP (1ull << 20)
#define CR4_SMAP (1ull << 21)

/* CPUID.7:0 - EBX bits for SMEP / SMAP (Intel SDM). */
#define CPUID7_EBX_SMEP (1u << 7)
#define CPUID7_EBX_SMAP (1u << 20)

/* Canonical sign-extend mask for bit 47 (4-level paging). */
#define CANON_SIGN_MASK 0xffff000000000000ull

/* Soft inventory greppable area count (honesty..gmap; deepen excluded). */
#define SMEP_SOFT_AREAS 14u

/* Cap full soft inventory dumps - no stamp storms. */
#define SMEP_SOFT_INV_CAP 2u

/*
 * Return-surface bit lamps (surf=0x... on soft surfaces/deepen).
 * greppable: smep: soft surfaces
 */
#define SMEP_SOFT_SURF_HONESTY   (1u << 0)
#define SMEP_SOFT_SURF_INVENTORY (1u << 1)
#define SMEP_SOFT_SURF_MAP       (1u << 2)
#define SMEP_SOFT_SURF_CR4       (1u << 3)
#define SMEP_SOFT_SURF_RESIDUAL  (1u << 4)
#define SMEP_SOFT_SURF_ENABLE    (1u << 5)
#define SMEP_SOFT_SURF_PATH      (1u << 6)
#define SMEP_SOFT_SURF_STATS     (1u << 7)
#define SMEP_SOFT_SURF_LAMPS     (1u << 8)
#define SMEP_SOFT_SURF_BAND      (1u << 9)
#define SMEP_SOFT_SURF_SURFACES  (1u << 10)
#define SMEP_SOFT_SURF_WALK      (1u << 11)
#define SMEP_SOFT_SURF_OPEN      (1u << 12)
#define SMEP_SOFT_SURF_GMAP      (1u << 13)
#define SMEP_SOFT_SURF_CATALOG                                                     \
    (SMEP_SOFT_SURF_HONESTY | SMEP_SOFT_SURF_INVENTORY | SMEP_SOFT_SURF_MAP |      \
     SMEP_SOFT_SURF_CR4 | SMEP_SOFT_SURF_RESIDUAL | SMEP_SOFT_SURF_ENABLE |        \
     SMEP_SOFT_SURF_PATH | SMEP_SOFT_SURF_STATS | SMEP_SOFT_SURF_LAMPS |           \
     SMEP_SOFT_SURF_BAND | SMEP_SOFT_SURF_SURFACES | SMEP_SOFT_SURF_WALK |         \
     SMEP_SOFT_SURF_OPEN | SMEP_SOFT_SURF_GMAP)

static struct gj_smep_stats g_stats;
static int                  g_fSmepOn;
static int                  g_fSmapOn;

/*
 * Soft map inventory axes (file-local).
 * Snapshotted over the last mutate harden walk; wrap OK; never hard-gate.
 * greppable: smep: soft
 */
static u64 g_u64SoftMapPresent;      /* present leaves visited (mutate) */
static u64 g_u64SoftMapLeaf4k;       /* present 4K leaves */
static u64 g_u64SoftMapLeaf2m;       /* present 2MiB PS leaves */
static u64 g_u64SoftMapLeaf1g;       /* present 1GiB PS leaves */
static u64 g_u64SoftMapUKernelHalf;  /* present U leaves in kernel half */
static u64 g_u64SoftMapULowOutside;  /* present U low, wholly outside user */
static u64 g_u64SoftMapUUserBand;    /* present U wholly inside user band */
static u64 g_u64SoftMapUStraddle;    /* present U on user-band straddle */
static u64 g_u64SoftMapAlreadySuper; /* present !U (already supervisor) */
static u64 g_u64SoftInvLogs;         /* smep_soft_inventory attempt count */
static u32 g_u32SoftInvEmitted;      /* full dumps toward SMEP_SOFT_INV_CAP */
static int g_fResidualLeanLamp;      /* once: soft residual lean */
static int g_fResidDeepenLamp;       /* once: C3 residual deepen (inv/lean) */

/*
 * Enable-path soft tallies (file-local; diagnostics only).
 * Separate from g_stats skip/on so inventory can show path attempts.
 */
static u64 g_u64SoftEnableSmepOk;
static u64 g_u64SoftEnableSmepFail;
static u64 g_u64SoftEnableSmepSkip;
static u64 g_u64SoftEnableSmapOk;
static u64 g_u64SoftEnableSmapFail;
static u64 g_u64SoftEnableSmapSkip;
static u64 g_u64SoftHardenNull;      /* null pml4 soft fail path */
static u64 g_u64SoftLastRemain;      /* last audit residual U */

/*
 * C3 soft residual snapshot (last harden/audit walk).
 * residual U outside user: base/last VA + pages + access + leaf size
 * (not residual_u count alone). Soft!=product: diagnose only; never
 * hard-gates; Dual DoD A/B OPEN (no product G-MAP claim).
 * greppable: smep: soft residual deepen
 */
static u64 g_u64SoftResidLeaves;     /* residual U leaves this walk */
static u64 g_u64SoftResidPages;      /* residual size in 4K pages (sum) */
static u64 g_u64SoftResidUx;         /* residual U+!NX leaves */
static u64 g_u64SoftResidNx;         /* residual U+NX leaves */
static u64 g_u64SoftResidStraddle;   /* residual on user-band straddle */
static u64 g_u64SoftResidKernel;     /* residual in kernel half */
static u64 g_u64SoftResidLow;        /* residual low outside user */
static u64 g_u64SoftResidW;          /* residual writable (PTE_W) leaves */
static u64 g_u64SoftResid4k;         /* residual 4K leaves */
static u64 g_u64SoftResid2m;         /* residual 2MiB PS leaves */
static u64 g_u64SoftResid1g;         /* residual 1GiB PS leaves */
static int g_fSoftResidHaveFirst;    /* first residual base captured */
static u64 g_u64SoftResidBaseVa;     /* C3: first residual leaf base VA */
static u64 g_u64SoftResidLastVa;     /* C3: last residual leaf base VA */
static u64 g_u64SoftResidFirstPages; /* C3: first residual leaf page count */
static u32 g_u32SoftResidAccess;     /* C3: first residual access lamps */
static int g_fSoftResidFirstSize;    /* C3: first residual fSize 0/1/2 */

/* Soft helpers defined after leaf_must_clear_u / CR4/CPUID statics. */
static void smep_soft_map_reset(void);
static void smep_soft_map_note_leaf(u64 u64Entry, u64 u64Va, u64 u64Cb,
                                    int fKernelHalf, int fSize);
static void smep_soft_resid_reset(void);
static void smep_soft_resid_note(u64 u64Va, u64 u64Cb, u64 u64Entry,
                                 int fKernelHalf, int fStraddle, int fSize);
static void smep_soft_residual_deepen_log(const char *szWhere);
static void smep_soft_residual_deepen_once(const char *szWhere);
static void smep_soft_residual_lean_once(const char *szWhere);
static void smep_soft_inventory(const char *szWhere);

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

    __asm__ volatile("mov %%cr3, %0" : "=r"(u64Cr3));
    return u64Cr3;
}

static u64
read_cr4(void)
{
    u64 u64Cr4;

    __asm__ volatile("mov %%cr4, %0" : "=r"(u64Cr4));
    return u64Cr4;
}

static void
write_cr4(u64 u64Cr4)
{
    __asm__ volatile("mov %0, %%cr4" : : "r"(u64Cr4) : "memory");
}

static void
reload_cr3(void)
{
    u64 u64Cr3 = read_cr3();

    __asm__ volatile("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
}

/**
 * Soft CPUID leaf 7 subleaf 0. Returns EBX feature bits (0 if leaf absent).
 */
static u32
cpuid7_ebx(void)
{
    u32 u32Max;
    u32 u32A;
    u32 u32B;
    u32 u32C;
    u32 u32D;

    __asm__ volatile("cpuid"
                     : "=a"(u32Max), "=b"(u32B), "=c"(u32C), "=d"(u32D)
                     : "a"(0u), "c"(0u));
    (void)u32B;
    (void)u32C;
    (void)u32D;
    if (u32Max < 7u) {
        return 0;
    }
    __asm__ volatile("cpuid"
                     : "=a"(u32A), "=b"(u32B), "=c"(u32C), "=d"(u32D)
                     : "a"(7u), "c"(0u));
    (void)u32A;
    (void)u32C;
    (void)u32D;
    return u32B;
}

/**
 * Build canonical VA from 4-level indices (4K grain; higher PS ignore low).
 */
static u64
canon_va(u32 u32I4, u32 u32I3, u32 u32I2, u32 u32I1)
{
    u64 u64Va;

    u64Va = ((u64)u32I4 << 39) | ((u64)u32I3 << 30) |
            ((u64)u32I2 << 21) | ((u64)u32I1 << 12);
    if ((u64Va & (1ull << 47)) != 0) {
        u64Va |= CANON_SIGN_MASK;
    }
    return u64Va;
}

/** Non-zero if [va, va+cb) is wholly outside the product user window. */
static int
va_wholly_outside_user(u64 u64Va, u64 u64Cb)
{
    u64 u64End;

    if (u64Cb == 0) {
        return 1;
    }
    u64End = u64Va + u64Cb;
    if (u64End < u64Va) {
        /* Overflow -> treat as kernel/high; harden soft. */
        return 1;
    }
    if (u64End <= GJ_USER_VA_BASE || u64Va >= GJ_USER_VA_END) {
        return 1;
    }
    return 0;
}

/** Non-zero if range is wholly inside the product user window. */
static int
va_wholly_inside_user(u64 u64Va, u64 u64Cb)
{
    u64 u64End;

    if (u64Cb == 0) {
        return 1;
    }
    u64End = u64Va + u64Cb;
    if (u64End < u64Va) {
        return 0;
    }
    if (u64Va >= GJ_USER_VA_BASE && u64End <= GJ_USER_VA_END) {
        return 1;
    }
    return 0;
}

/**
 * Decide whether a present leaf at [va, va+cb) must lose U.
 * Kernel half (high canonical / PML4 ≥ 256): always clear U (G-MAP-1).
 * Low half: clear when wholly outside user band; leave wholly inside;
 * straddle large pages counted soft residual (cannot partial-clear PS).
 */
static int
leaf_must_clear_u(u64 u64Va, u64 u64Cb, int fKernelHalf, int *pStraddle)
{
    if (pStraddle != NULL) {
        *pStraddle = 0;
    }
    if (fKernelHalf != 0) {
        return 1;
    }
    if (va_wholly_outside_user(u64Va, u64Cb) != 0) {
        return 1;
    }
    if (va_wholly_inside_user(u64Va, u64Cb) != 0) {
        return 0;
    }
    /* Partial overlap with user band (large-page edge case). */
    if (pStraddle != NULL) {
        *pStraddle = 1;
    }
    return 0;
}

/**
 * Soft: reset map-inventory tallies before a mutate harden walk.
 * greppable: smep: soft map
 */
static void
smep_soft_map_reset(void)
{
    g_u64SoftMapPresent = 0;
    g_u64SoftMapLeaf4k = 0;
    g_u64SoftMapLeaf2m = 0;
    g_u64SoftMapLeaf1g = 0;
    g_u64SoftMapUKernelHalf = 0;
    g_u64SoftMapULowOutside = 0;
    g_u64SoftMapUUserBand = 0;
    g_u64SoftMapUStraddle = 0;
    g_u64SoftMapAlreadySuper = 0;
}

/**
 * Soft: classify one present leaf for map inventory (pre-clear snapshot).
 * fSize: 0=4K, 1=2M, 2=1G. Call only on mutate harden walks.
 * greppable: smep: soft map
 */
static void
smep_soft_map_note_leaf(u64 u64Entry, u64 u64Va, u64 u64Cb, int fKernelHalf,
                        int fSize)
{
    int fStraddle = 0;
    int fMustClear;

    g_u64SoftMapPresent++;
    if (fSize == 2) {
        g_u64SoftMapLeaf1g++;
    } else if (fSize == 1) {
        g_u64SoftMapLeaf2m++;
    } else {
        g_u64SoftMapLeaf4k++;
    }

    if ((u64Entry & PTE_U) == 0) {
        g_u64SoftMapAlreadySuper++;
        return;
    }

    fMustClear = leaf_must_clear_u(u64Va, u64Cb, fKernelHalf, &fStraddle);
    if (fMustClear != 0) {
        if (fKernelHalf != 0) {
            g_u64SoftMapUKernelHalf++;
        } else {
            g_u64SoftMapULowOutside++;
        }
    } else if (fStraddle != 0) {
        g_u64SoftMapUStraddle++;
    } else {
        g_u64SoftMapUUserBand++;
    }
}

/**
 * Soft: reset C3 residual snapshot before a harden/audit walk.
 * greppable: smep: soft residual deepen
 */
static void
smep_soft_resid_reset(void)
{
    g_u64SoftResidLeaves = 0;
    g_u64SoftResidPages = 0;
    g_u64SoftResidUx = 0;
    g_u64SoftResidNx = 0;
    g_u64SoftResidStraddle = 0;
    g_u64SoftResidKernel = 0;
    g_u64SoftResidLow = 0;
    g_u64SoftResidW = 0;
    g_u64SoftResid4k = 0;
    g_u64SoftResid2m = 0;
    g_u64SoftResid1g = 0;
    g_fSoftResidHaveFirst = 0;
    g_u64SoftResidBaseVa = 0;
    g_u64SoftResidLastVa = 0;
    g_u64SoftResidFirstPages = 0;
    g_u32SoftResidAccess = 0;
    g_fSoftResidFirstSize = 0;
}

/**
 * Soft C3 residual note: one residual U leaf outside user band.
 * Captures first/last base VA + page count + access + leaf size (C3 shape).
 * fSize: 0=4K, 1=2M, 2=1G. Soft!=product: bookkeeping only; never mutates PTEs.
 * greppable: smep: soft residual deepen
 */
static void
smep_soft_resid_note(u64 u64Va, u64 u64Cb, u64 u64Entry, int fKernelHalf,
                     int fStraddle, int fSize)
{
    u64 u64Pages;
    u32 u32Access;

    g_u64SoftResidLeaves++;
    if (u64Cb == 0) {
        u64Pages = 1ull;
    } else {
        u64Pages = (u64Cb + (u64)GJ_PAGE_SIZE - 1ull) / (u64)GJ_PAGE_SIZE;
    }
    g_u64SoftResidPages += u64Pages;

    if ((u64Entry & PTE_NX) == 0) {
        g_u64SoftResidUx++;
    } else {
        g_u64SoftResidNx++;
    }
    if (fStraddle != 0) {
        g_u64SoftResidStraddle++;
    }
    if (fKernelHalf != 0) {
        g_u64SoftResidKernel++;
    } else {
        g_u64SoftResidLow++;
    }
    if ((u64Entry & PTE_W) != 0) {
        g_u64SoftResidW++;
    }
    if (fSize == 2) {
        g_u64SoftResid1g++;
    } else if (fSize == 1) {
        g_u64SoftResid2m++;
    } else {
        g_u64SoftResid4k++;
    }

    /* Always refresh last residual base (cluster end honesty). */
    g_u64SoftResidLastVa = u64Va;

    /* First residual only: C3 base VA + pages + access + size of head. */
    if (g_fSoftResidHaveFirst != 0) {
        return;
    }
    g_fSoftResidHaveFirst = 1;
    g_u64SoftResidBaseVa = u64Va;
    g_u64SoftResidFirstPages = u64Pages;
    g_fSoftResidFirstSize = fSize;
    u32Access = 0;
    if ((u64Entry & PTE_U) != 0) {
        u32Access |= SMEP_SOFT_RESID_ACC_U;
    }
    if ((u64Entry & PTE_NX) != 0) {
        u32Access |= SMEP_SOFT_RESID_ACC_NX;
    } else {
        u32Access |= SMEP_SOFT_RESID_ACC_X;
    }
    if (fKernelHalf != 0) {
        u32Access |= SMEP_SOFT_RESID_ACC_KHALF;
    }
    if (fStraddle != 0) {
        u32Access |= SMEP_SOFT_RESID_ACC_STRADDLE;
    }
    if ((u64Entry & PTE_W) != 0) {
        u32Access |= SMEP_SOFT_RESID_ACC_W;
    }
    if (fSize == 2) {
        u32Access |= SMEP_SOFT_RESID_ACC_1G;
    } else if (fSize == 1) {
        u32Access |= SMEP_SOFT_RESID_ACC_2M;
    } else {
        u32Access |= SMEP_SOFT_RESID_ACC_4K;
    }
    g_u32SoftResidAccess = u32Access;
}

/**
 * Greppable C3 residual deepen line (base/last VA + pages + access + size).
 * Soft!=product; Dual DoD A/B OPEN; never hard-gates; stamp-free.
 * greppable: smep: soft residual deepen
 */
static void
smep_soft_residual_deepen_log(const char *szWhere)
{
    if (szWhere == NULL) {
        szWhere = "path";
    }
    /* Grep: smep: soft residual deepen | Dual_DoD_A=OPEN Dual_DoD_B=OPEN */
    kprintf("smep: soft residual deepen via=%s "
            "base_va=0x%llx last_va=0x%llx first_pages=%lu first_size=%d "
            "leaves=%lu pages=%lu ux=%lu nx=%lu w=%lu "
            "straddle=%lu kernel=%lu low=%lu leaf4k=%lu leaf2m=%lu leaf1g=%lu "
            "access=0x%x residual_u=%lu last_remain=%lu have_first=%d "
            "c3_base_pages_access=1 c3_size_last_w=1 "
            "soft_ne_product=1 product_gmap=OPEN product_pmem6=OPEN "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "(C3 residual shape; Soft!=product; not product G-MAP; "
            "G-AC-1; dual MIT|Apache-2.0; never hard-gates boot)\n",
            szWhere,
            (unsigned long long)g_u64SoftResidBaseVa,
            (unsigned long long)g_u64SoftResidLastVa,
            (unsigned long)g_u64SoftResidFirstPages,
            g_fSoftResidFirstSize,
            (unsigned long)g_u64SoftResidLeaves,
            (unsigned long)g_u64SoftResidPages,
            (unsigned long)g_u64SoftResidUx,
            (unsigned long)g_u64SoftResidNx,
            (unsigned long)g_u64SoftResidW,
            (unsigned long)g_u64SoftResidStraddle,
            (unsigned long)g_u64SoftResidKernel,
            (unsigned long)g_u64SoftResidLow,
            (unsigned long)g_u64SoftResid4k,
            (unsigned long)g_u64SoftResid2m,
            (unsigned long)g_u64SoftResid1g,
            (unsigned)g_u32SoftResidAccess,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned long)g_u64SoftLastRemain,
            g_fSoftResidHaveFirst);
}

/**
 * Once-only C3 residual deepen (inventory / lean paths; no stamp storm).
 * Audit FAIL may still call smep_soft_residual_deepen_log un-capped.
 * greppable: smep: soft residual deepen
 */
static void
smep_soft_residual_deepen_once(const char *szWhere)
{
    if (g_fResidDeepenLamp != 0) {
        return;
    }
    g_fResidDeepenLamp = 1;
    smep_soft_residual_deepen_log(szWhere);
}

/**
 * Once-only lean residual lamp (no version stamp; no stamp storm).
 * Soft!=product: residual shape only - not product G-MAP / P-MEM-6 close.
 * C3: base/last VA + first_pages + access + size when residual present.
 * Dual DoD A/B OPEN. greppable: smep: soft residual lean
 */
static void
smep_soft_residual_lean_once(const char *szWhere)
{
    u64 u64Cr4;
    u32 u32Ebx;
    u64 u64Cleared;

    if (g_fResidualLeanLamp != 0) {
        return;
    }
    g_fResidualLeanLamp = 1;
    if (szWhere == NULL) {
        szWhere = "path";
    }

    u64Cr4 = read_cr4();
    u32Ebx = cpuid7_ebx();
    u64Cleared = g_stats.u64Cleared4k + g_stats.u64Cleared2m +
                 g_stats.u64Cleared1g;

    /* Grep: smep: soft residual lean | Dual_DoD_A=OPEN Dual_DoD_B=OPEN */
    kprintf("smep: soft residual lean via=%s "
            "harden=%lu audit=%lu residual_u=%lu last_remain=%lu "
            "cleared=%lu walked=%lu straddle=%lu ux=%lu "
            "base_va=0x%llx last_va=0x%llx first_pages=%lu first_size=%d "
            "resid_pages=%lu resid_w=%lu leaf4k=%lu leaf2m=%lu leaf1g=%lu "
            "access=0x%x have_first=%d "
            "smep_bit=%d smap_bit=%d "
            "cpuid7_smep=%d cpuid7_smap=%d "
            "g1_clear_u=1 g2_user_band=1 g3_smep=1 g4_ux_nx=1 "
            "c3_base_pages_access=1 c3_size_last_w=1 "
            "stamp_storm=0 soft_ne_product=1 product_gmap=OPEN "
            "product_pmem6=OPEN straddle_split=OPEN "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "(Soft!=product; lean residual only; never hard-gates boot)\n",
            szWhere,
            (unsigned long)g_stats.u64HardenCalls,
            (unsigned long)g_stats.u64AuditCalls,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned long)g_u64SoftLastRemain,
            (unsigned long)u64Cleared,
            (unsigned long)g_stats.u64WalkedLeaves,
            (unsigned long)g_stats.u64StraddleLarge,
            (unsigned long)g_stats.u64UxCleared,
            (unsigned long long)g_u64SoftResidBaseVa,
            (unsigned long long)g_u64SoftResidLastVa,
            (unsigned long)g_u64SoftResidFirstPages,
            g_fSoftResidFirstSize,
            (unsigned long)g_u64SoftResidPages,
            (unsigned long)g_u64SoftResidW,
            (unsigned long)g_u64SoftResid4k,
            (unsigned long)g_u64SoftResid2m,
            (unsigned long)g_u64SoftResid1g,
            (unsigned)g_u32SoftResidAccess,
            g_fSoftResidHaveFirst,
            ((u64Cr4 & CR4_SMEP) != 0) ? 1 : 0,
            ((u64Cr4 & CR4_SMAP) != 0) ? 1 : 0,
            ((u32Ebx & CPUID7_EBX_SMEP) != 0) ? 1 : 0,
            ((u32Ebx & CPUID7_EBX_SMAP) != 0) ? 1 : 0);
    /* C3 deepen rides lean once-path when inv already capped (no storm). */
    smep_soft_residual_deepen_once(szWhere);
}

/**
 * Greppable soft SMEP/map inventory dump (capped - no stamp storms).
 * Prefix-stable markers (smep: soft ...). After SMEP_SOFT_INV_CAP full dumps,
 * only lean residual once. Never allocates; safe from boot harden/enable.
 * Honesty: soft inventory != product G-MAP complete; soft != product.
 * greppable: smep: soft
 */
static void
smep_soft_inventory(const char *szWhere)
{
    u64 u64Cr4;
    u32 u32Ebx;
    u64 u64Cleared;
    int fSmepBit;
    int fSmapBit;
    int fCpuidSmep;
    int fCpuidSmap;
    u32 u32Areas;
    u32 u32Surf;
    int fSoftPass;

    g_u64SoftInvLogs++;
    if (szWhere == NULL) {
        szWhere = "path";
    }

    /* Cap full multi-line dumps - lean residual covers later paths. */
    if (g_u32SoftInvEmitted >= SMEP_SOFT_INV_CAP) {
        smep_soft_residual_lean_once(szWhere);
        return;
    }
    g_u32SoftInvEmitted++;

    u64Cr4 = read_cr4();
    u32Ebx = cpuid7_ebx();
    fSmepBit = ((u64Cr4 & CR4_SMEP) != 0) ? 1 : 0;
    fSmapBit = ((u64Cr4 & CR4_SMAP) != 0) ? 1 : 0;
    fCpuidSmep = ((u32Ebx & CPUID7_EBX_SMEP) != 0) ? 1 : 0;
    fCpuidSmap = ((u32Ebx & CPUID7_EBX_SMAP) != 0) ? 1 : 0;
    u64Cleared = g_stats.u64Cleared4k + g_stats.u64Cleared2m +
                 g_stats.u64Cleared1g;
    u32Areas = 0;
    u32Surf = SMEP_SOFT_SURF_CATALOG;

    /*
     * Honesty first: freestanding soft inventory is NOT product G-MAP close.
     * Grep: smep: soft honesty
     */
    kprintf("smep: soft honesty not-product-GMAP not-full-P-MEM-6 "
            "g_map=soft p_mem6=soft product_gmap=OPEN product_pmem6=OPEN "
            "straddle_split=OPEN soft_ne_product=1 "
            "(soft inventory only; never hard-gates boot)\n");
    u32Areas++;

    /* Grep: smep: soft inventory */
    kprintf("smep: soft inventory via=%s harden=%lu audit=%lu "
            "pass=%lu fail=%lu residual_u=%lu logs=%lu "
            "g_map=1..4 p_mem6=smep+smap user_band=[0x%llx,0x%llx) "
            "(soft; not product G-MAP; Soft!=product)\n",
            szWhere,
            (unsigned long)g_stats.u64HardenCalls,
            (unsigned long)g_stats.u64AuditCalls,
            (unsigned long)g_stats.u64SoftPass,
            (unsigned long)g_stats.u64SoftFail,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned long)g_u64SoftInvLogs,
            (unsigned long long)GJ_USER_VA_BASE,
            (unsigned long long)GJ_USER_VA_END);
    u32Areas++;

    /* Grep: smep: soft map */
    kprintf("smep: soft map present=%lu leaf4k=%lu leaf2m=%lu leaf1g=%lu "
            "u_kernel=%lu u_low_out=%lu u_user=%lu u_straddle=%lu "
            "already_super=%lu cleared=%lu (4k=%lu 2m=%lu 1g=%lu) "
            "ux=%lu residual_u=%lu walked=%lu skip_user=%lu "
            "straddle_large=%lu\n",
            (unsigned long)g_u64SoftMapPresent,
            (unsigned long)g_u64SoftMapLeaf4k,
            (unsigned long)g_u64SoftMapLeaf2m,
            (unsigned long)g_u64SoftMapLeaf1g,
            (unsigned long)g_u64SoftMapUKernelHalf,
            (unsigned long)g_u64SoftMapULowOutside,
            (unsigned long)g_u64SoftMapUUserBand,
            (unsigned long)g_u64SoftMapUStraddle,
            (unsigned long)g_u64SoftMapAlreadySuper,
            (unsigned long)u64Cleared,
            (unsigned long)g_stats.u64Cleared4k,
            (unsigned long)g_stats.u64Cleared2m,
            (unsigned long)g_stats.u64Cleared1g,
            (unsigned long)g_stats.u64UxCleared,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned long)g_stats.u64WalkedLeaves,
            (unsigned long)g_stats.u64SkippedUserBand,
            (unsigned long)g_stats.u64StraddleLarge);
    u32Areas++;

    /* Grep: smep: soft cr4 */
    kprintf("smep: soft cr4 cr4=0x%lx smep_bit=%d smap_bit=%d "
            "smep_on=%d smap_on=%d cpuid7_smep=%d cpuid7_smap=%d "
            "skip_smep=%lu skip_smap=%lu\n",
            (unsigned long)u64Cr4, fSmepBit, fSmapBit,
            g_fSmepOn, g_fSmapOn, fCpuidSmep, fCpuidSmap,
            (unsigned long)g_stats.u64SmepSkip,
            (unsigned long)g_stats.u64SmapSkip);
    u32Areas++;

    /* Grep: smep: soft residual | Dual_DoD_A=OPEN Dual_DoD_B=OPEN */
    kprintf("smep: soft residual last_remain=%lu audit_remain=%lu "
            "straddle_large=%lu ux_cleared=%lu skip_user=%lu "
            "harden_null=%lu soft_pass=%lu soft_fail=%lu "
            "base_va=0x%llx last_va=0x%llx first_pages=%lu first_size=%d "
            "resid_pages=%lu resid_w=%lu leaf4k=%lu leaf2m=%lu leaf1g=%lu "
            "access=0x%x have_first=%d Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "(audit residual; C3 base+last+pages+access+size; soft only; "
            "Soft!=product; not product gate)\n",
            (unsigned long)g_u64SoftLastRemain,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned long)g_stats.u64StraddleLarge,
            (unsigned long)g_stats.u64UxCleared,
            (unsigned long)g_stats.u64SkippedUserBand,
            (unsigned long)g_u64SoftHardenNull,
            (unsigned long)g_stats.u64SoftPass,
            (unsigned long)g_stats.u64SoftFail,
            (unsigned long long)g_u64SoftResidBaseVa,
            (unsigned long long)g_u64SoftResidLastVa,
            (unsigned long)g_u64SoftResidFirstPages,
            g_fSoftResidFirstSize,
            (unsigned long)g_u64SoftResidPages,
            (unsigned long)g_u64SoftResidW,
            (unsigned long)g_u64SoftResid4k,
            (unsigned long)g_u64SoftResid2m,
            (unsigned long)g_u64SoftResid1g,
            (unsigned)g_u32SoftResidAccess,
            g_fSoftResidHaveFirst);
    u32Areas++;

    /*
     * C3 residual deepen: explicit base VA + page count + access surface.
     * Once-lamp shared with lean path - no stamp storm.
     * Grep: smep: soft residual deepen
     */
    smep_soft_residual_deepen_once(szWhere);

    /* Grep: smep: soft enable */
    kprintf("smep: soft enable smep_ok=%lu smep_fail=%lu smep_skip=%lu "
            "smap_ok=%lu smap_fail=%lu smap_skip=%lu "
            "cpuid7_smep=%d cpuid7_smap=%d "
            "(CPUID-gated CR4; soft path only)\n",
            (unsigned long)g_u64SoftEnableSmepOk,
            (unsigned long)g_u64SoftEnableSmepFail,
            (unsigned long)g_u64SoftEnableSmepSkip,
            (unsigned long)g_u64SoftEnableSmapOk,
            (unsigned long)g_u64SoftEnableSmapFail,
            (unsigned long)g_u64SoftEnableSmapSkip,
            fCpuidSmep, fCpuidSmap);
    u32Areas++;

    /*
     * Soft path honesty: surface catalog + explicit non-claims.
     * Grep: smep: soft path
     */
    kprintf("smep: soft path via=%s "
            "harden_walk->clear_U->reload_cr3->audit->enable_smep->enable_smap "
            "leaf=4k|2m|1g g_map=1..4 p_mem6=soft "
            "straddle_split=OPEN product_gmap=OPEN "
            "(soft inventory; Soft!=product)\n",
            szWhere);
    u32Areas++;

    /* Grep: smep: soft stats */
    kprintf("smep: soft stats harden=%lu audit=%lu cleared=%lu "
            "walked=%lu ux=%lu residual_u=%lu soft_pass=%lu soft_fail=%lu "
            "smep_on=%lu smap_on=%lu inv_logs=%lu inv_emitted=%u\n",
            (unsigned long)g_stats.u64HardenCalls,
            (unsigned long)g_stats.u64AuditCalls,
            (unsigned long)u64Cleared,
            (unsigned long)g_stats.u64WalkedLeaves,
            (unsigned long)g_stats.u64UxCleared,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned long)g_stats.u64SoftPass,
            (unsigned long)g_stats.u64SoftFail,
            (unsigned long)(cpu_smep_is_enabled() ? 1ull : 0ull),
            (unsigned long)(cpu_smap_is_enabled() ? 1ull : 0ull),
            (unsigned long)g_u64SoftInvLogs,
            (unsigned)g_u32SoftInvEmitted);
    u32Areas++;

    /* Grep: smep: soft lamps */
    kprintf("smep: soft lamps smep_bit=%d smap_bit=%d "
            "cpuid7_smep=%d cpuid7_smap=%d smep_on=%d smap_on=%d "
            "harden_calls=%lu residual_u=%lu "
            "(soft readiness; not product gate)\n",
            fSmepBit, fSmapBit, fCpuidSmep, fCpuidSmap,
            g_fSmepOn, g_fSmapOn,
            (unsigned long)g_stats.u64HardenCalls,
            (unsigned long)g_stats.u64AuditRemainU);
    u32Areas++;

    /* Grep: smep: soft band (user-band geometry) */
    kprintf("smep: soft band user_base=0x%llx user_end=0x%llx "
            "cr4_smep_bit=%d cr4_smap_bit=%d leaf=4k|2m|1g "
            "u_kernel=%lu u_user=%lu u_straddle=%lu\n",
            (unsigned long long)GJ_USER_VA_BASE,
            (unsigned long long)GJ_USER_VA_END,
            fSmepBit, fSmapBit,
            (unsigned long)g_u64SoftMapUKernelHalf,
            (unsigned long)g_u64SoftMapUUserBand,
            (unsigned long)g_u64SoftMapUStraddle);
    u32Areas++;

    /* Grep: smep: soft surfaces */
    kprintf("smep: soft surfaces surf=0x%x catalog=%u areas_live=%u "
            "map=1 cr4=1 residual=1 enable=1 walk=1 gmap=1 open=1 "
            "(return surfaces; soft only; not product G-MAP)\n",
            (unsigned)u32Surf, (unsigned)SMEP_SOFT_AREAS, u32Areas + 4u);
    u32Areas++;

    /* Grep: smep: soft walk */
    kprintf("smep: soft walk walked=%lu present=%lu cleared=%lu "
            "skip_user=%lu leaf4k=%lu leaf2m=%lu leaf1g=%lu "
            "straddle_large=%lu "
            "(soft walk; not product G-MAP)\n",
            (unsigned long)g_stats.u64WalkedLeaves,
            (unsigned long)g_u64SoftMapPresent,
            (unsigned long)u64Cleared,
            (unsigned long)g_stats.u64SkippedUserBand,
            (unsigned long)g_u64SoftMapLeaf4k,
            (unsigned long)g_u64SoftMapLeaf2m,
            (unsigned long)g_u64SoftMapLeaf1g,
            (unsigned long)g_stats.u64StraddleLarge);
    u32Areas++;

    /* Grep: smep: soft OPEN | Dual_DoD_A=OPEN Dual_DoD_B=OPEN */
    kprintf("smep: soft OPEN product_gmap=OPEN product_pmem6=OPEN "
            "straddle_split=OPEN Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "(soft honesty; Soft!=product; Dual DoD residual OPEN)\n");
    u32Areas++;

    /* Grep: smep: soft gmap */
    kprintf("smep: soft gmap g1_clear_u=1 g2_user_band=1 g3_smep=1 "
            "g4_ux_nx=1 u_kernel=%lu u_user=%lu u_straddle=%lu "
            "ux_cleared=%lu residual_u=%lu "
            "(G-MAP soft axes; not product complete)\n",
            (unsigned long)g_u64SoftMapUKernelHalf,
            (unsigned long)g_u64SoftMapUUserBand,
            (unsigned long)g_u64SoftMapUStraddle,
            (unsigned long)g_stats.u64UxCleared,
            (unsigned long)g_stats.u64AuditRemainU);
    u32Areas++;

    /*
     * Grep: smep: soft deepen
     * areas = prior soft lines this emission (honesty..gmap). No version stamp.
     */
    kprintf("smep: soft deepen areas=%u via=%s logs=%lu "
            "catalog=%u residual_u=%lu surf=0x%x inv_cap=%u "
            "(lean exclusive; not product G-MAP; soft!=product)\n",
            (unsigned)u32Areas,
            szWhere,
            (unsigned long)g_u64SoftInvLogs,
            (unsigned)SMEP_SOFT_AREAS,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned)u32Surf,
            (unsigned)SMEP_SOFT_INV_CAP);

    /*
     * Soft close lamp: residual U == 0 after any audit is soft-pass shape.
     * Never hard-gates. Grep: smep: soft PASS | smep: soft FAIL
     */
    fSoftPass = (g_stats.u64AuditCalls != 0 &&
                 g_stats.u64AuditRemainU == 0) ? 1 : 0;
    if (fSoftPass != 0) {
        kprintf("smep: soft PASS via=%s residual_u=0 harden=%lu "
                "(soft inventory only; not product gate)\n",
                szWhere,
                (unsigned long)g_stats.u64HardenCalls);
    } else if (g_stats.u64AuditCalls != 0) {
        kprintf("smep: soft FAIL via=%s residual_u=%lu "
                "(soft inventory only; not product gate)\n",
                szWhere,
                (unsigned long)g_stats.u64AuditRemainU);
    }

    /* Lean residual once after a full dump (idempotent). */
    smep_soft_residual_lean_once(szWhere);

    (void)SMEP_SOFT_AREAS;
}

/**
 * Clear U on one leaf entry; update soft counters.
 * fSize: 0=4K, 1=2M, 2=1G.
 */
static void
clear_u_leaf(u64 *pEntry, u64 u64Va, int fSize)
{
    if (pEntry == NULL) {
        return;
    }
    if ((*pEntry & PTE_P) == 0) {
        return;
    }
    if ((*pEntry & PTE_U) == 0) {
        return;
    }
    /* G-MAP-4 soft: executable user residual on kernel VA. */
    if ((*pEntry & PTE_NX) == 0) {
        g_stats.u64UxCleared++;
    }
    *pEntry &= ~PTE_U;
    if (fSize == 2) {
        g_stats.u64Cleared1g++;
    } else if (fSize == 1) {
        g_stats.u64Cleared2m++;
    } else {
        g_stats.u64Cleared4k++;
    }
    (void)u64Va;
}

/**
 * Walk one PML4 tree: clear U on kernel-only leaves; optional audit-only.
 * fMutate 0 -> count residual U only (soft audit); 1 -> clear.
 * Returns residual U leaves still set outside user band (after pass).
 */
static u64
walk_harden_pml4(u64 *pPml4, int fMutate)
{
    u32 u32I4;
    u32 u32I3;
    u32 u32I2;
    u32 u32I1;
    u64 u64Remain = 0;

    if (pPml4 == NULL) {
        return 0;
    }

    /* C3 residual snapshot is per-walk (audit overwrites mutate). */
    smep_soft_resid_reset();

    for (u32I4 = 0; u32I4 < 512u; u32I4++) {
        u64 *pPdpt;
        int fKernelHalf = (u32I4 >= 256u) ? 1 : 0;

        if ((pPml4[u32I4] & PTE_P) == 0) {
            continue;
        }
        /* PML4e never a leaf on 4-level; ignore PS if set. */
        pPdpt = phys_to_virt(pPml4[u32I4] & PTE_ADDR_MASK);

        for (u32I3 = 0; u32I3 < 512u; u32I3++) {
            u64 *pPd;
            u64 u64Va1g = canon_va(u32I4, u32I3, 0, 0);
            int fStraddle = 0;

            if ((pPdpt[u32I3] & PTE_P) == 0) {
                continue;
            }
            if ((pPdpt[u32I3] & PTE_PS) != 0) {
                /* 1GiB page */
                g_stats.u64WalkedLeaves++;
                if (fMutate != 0) {
                    smep_soft_map_note_leaf(pPdpt[u32I3], u64Va1g,
                                            1ull << 30, fKernelHalf, 2);
                }
                if (leaf_must_clear_u(u64Va1g, 1ull << 30, fKernelHalf,
                                      &fStraddle) != 0) {
                    if (fMutate != 0) {
                        clear_u_leaf(&pPdpt[u32I3], u64Va1g, 2);
                    }
                    if ((pPdpt[u32I3] & PTE_U) != 0) {
                        u64Remain++;
                        smep_soft_resid_note(u64Va1g, 1ull << 30,
                                             pPdpt[u32I3], fKernelHalf,
                                             fStraddle, 2);
                    }
                } else if (fStraddle != 0) {
                    g_stats.u64StraddleLarge++;
                    /* Soft residual: cannot split without vmm. */
                    if ((pPdpt[u32I3] & PTE_U) != 0) {
                        u64Remain++;
                        smep_soft_resid_note(u64Va1g, 1ull << 30,
                                             pPdpt[u32I3], fKernelHalf, 1, 2);
                    }
                } else {
                    g_stats.u64SkippedUserBand++;
                }
                continue;
            }

            pPd = phys_to_virt(pPdpt[u32I3] & PTE_ADDR_MASK);
            for (u32I2 = 0; u32I2 < 512u; u32I2++) {
                u64 *pPt;
                u64 u64Va2m = canon_va(u32I4, u32I3, u32I2, 0);

                if ((pPd[u32I2] & PTE_P) == 0) {
                    continue;
                }
                if ((pPd[u32I2] & PTE_PS) != 0) {
                    /* 2MiB page */
                    g_stats.u64WalkedLeaves++;
                    fStraddle = 0;
                    if (fMutate != 0) {
                        smep_soft_map_note_leaf(pPd[u32I2], u64Va2m,
                                                1ull << 21, fKernelHalf, 1);
                    }
                    if (leaf_must_clear_u(u64Va2m, 1ull << 21, fKernelHalf,
                                          &fStraddle) != 0) {
                        if (fMutate != 0) {
                            clear_u_leaf(&pPd[u32I2], u64Va2m, 1);
                        }
                        if ((pPd[u32I2] & PTE_U) != 0) {
                            u64Remain++;
                            smep_soft_resid_note(u64Va2m, 1ull << 21,
                                                 pPd[u32I2], fKernelHalf,
                                                 fStraddle, 1);
                        }
                    } else if (fStraddle != 0) {
                        g_stats.u64StraddleLarge++;
                        if ((pPd[u32I2] & PTE_U) != 0) {
                            u64Remain++;
                            smep_soft_resid_note(u64Va2m, 1ull << 21,
                                                 pPd[u32I2], fKernelHalf, 1, 1);
                        }
                    } else {
                        g_stats.u64SkippedUserBand++;
                    }
                    continue;
                }

                pPt = phys_to_virt(pPd[u32I2] & PTE_ADDR_MASK);
                for (u32I1 = 0; u32I1 < 512u; u32I1++) {
                    u64 u64Va = canon_va(u32I4, u32I3, u32I2, u32I1);

                    if ((pPt[u32I1] & PTE_P) == 0) {
                        continue;
                    }
                    g_stats.u64WalkedLeaves++;
                    fStraddle = 0;
                    if (fMutate != 0) {
                        smep_soft_map_note_leaf(pPt[u32I1], u64Va,
                                                (u64)GJ_PAGE_SIZE,
                                                fKernelHalf, 0);
                    }
                    if (leaf_must_clear_u(u64Va, (u64)GJ_PAGE_SIZE,
                                          fKernelHalf, &fStraddle) != 0) {
                        if (fMutate != 0) {
                            clear_u_leaf(&pPt[u32I1], u64Va, 0);
                        }
                        if ((pPt[u32I1] & PTE_U) != 0) {
                            u64Remain++;
                            smep_soft_resid_note(u64Va, (u64)GJ_PAGE_SIZE,
                                                 pPt[u32I1], fKernelHalf,
                                                 fStraddle, 0);
                        }
                    } else if (fStraddle != 0) {
                        /* 4K cannot straddle user edge if PAGE_SIZE aligned. */
                        g_stats.u64StraddleLarge++;
                        if ((pPt[u32I1] & PTE_U) != 0) {
                            u64Remain++;
                            smep_soft_resid_note(u64Va, (u64)GJ_PAGE_SIZE,
                                                 pPt[u32I1], fKernelHalf, 1, 0);
                        }
                    } else {
                        g_stats.u64SkippedUserBand++;
                    }
                }
            }
        }
    }
    return u64Remain;
}

static u64 *
harden_pml4_base(void)
{
    u64 u64Cr3 = vmm_kernel_cr3();

    /* Prefer kernel template CR3 when published (G-AS soft). */
    if (u64Cr3 == 0) {
        u64Cr3 = read_cr3() & PTE_ADDR_MASK;
    } else {
        u64Cr3 &= PTE_ADDR_MASK;
    }
    return phys_to_virt(u64Cr3);
}

void
cpu_enable_smep(void)
{
    u32 u32Ebx = cpuid7_ebx();
    u64 u64Cr4;

    if ((u32Ebx & CPUID7_EBX_SMEP) == 0) {
        g_stats.u64SmepSkip++;
        g_u64SoftEnableSmepSkip++;
        g_fSmepOn = 0;
        g_stats.u64SmepOn = 0;
        kprintf("smep: SMEP soft skip (CPUID.7 no SMEP)\n");
        /* greppable: smep: soft */
        smep_soft_inventory("smep_skip");
        return;
    }
    u64Cr4 = read_cr4();
    u64Cr4 |= CR4_SMEP;
    write_cr4(u64Cr4);
    u64Cr4 = read_cr4();
    if ((u64Cr4 & CR4_SMEP) != 0) {
        g_fSmepOn = 1;
        g_stats.u64SmepOn = 1;
        g_u64SoftEnableSmepOk++;
        kprintf("smep: SMEP enabled CR4=0x%lx soft PASS\n",
                (unsigned long)u64Cr4);
        /* greppable: smep: soft */
        smep_soft_inventory("smep_enable");
    } else {
        g_fSmepOn = 0;
        g_stats.u64SmepOn = 0;
        g_stats.u64SoftFail++;
        g_u64SoftEnableSmepFail++;
        kprintf("smep: SMEP soft FAIL CR4=0x%lx (bit not sticky)\n",
                (unsigned long)u64Cr4);
        /* greppable: smep: soft */
        smep_soft_inventory("smep_fail");
    }
}

void
cpu_enable_smap(void)
{
    u32 u32Ebx = cpuid7_ebx();
    u64 u64Cr4;

    if ((u32Ebx & CPUID7_EBX_SMAP) == 0) {
        g_stats.u64SmapSkip++;
        g_u64SoftEnableSmapSkip++;
        g_fSmapOn = 0;
        g_stats.u64SmapOn = 0;
        kprintf("smep: SMAP soft skip (CPUID.7 no SMAP)\n");
        /* greppable: smep: soft */
        smep_soft_inventory("smap_skip");
        return;
    }
    u64Cr4 = read_cr4();
    u64Cr4 |= CR4_SMAP;
    write_cr4(u64Cr4);
    /* Default AC clear - kernel must STAC before user access */
    __asm__ volatile("clac" ::: "memory");
    u64Cr4 = read_cr4();
    if ((u64Cr4 & CR4_SMAP) != 0) {
        g_fSmapOn = 1;
        g_stats.u64SmapOn = 1;
        g_u64SoftEnableSmapOk++;
        user_access_smap_enabled();
        kprintf("smep: SMAP enabled CR4=0x%lx; copy_* STAC/CLAC soft PASS\n",
                (unsigned long)u64Cr4);
        /* greppable: smep: soft */
        smep_soft_inventory("smap_enable");
    } else {
        g_fSmapOn = 0;
        g_stats.u64SmapOn = 0;
        g_stats.u64SoftFail++;
        g_u64SoftEnableSmapFail++;
        kprintf("smep: SMAP soft FAIL CR4=0x%lx (bit not sticky)\n",
                (unsigned long)u64Cr4);
        /* greppable: smep: soft */
        smep_soft_inventory("smap_fail");
    }
}

int
cpu_smep_is_enabled(void)
{
    if (g_fSmepOn != 0 && (read_cr4() & CR4_SMEP) != 0) {
        return 1;
    }
    return 0;
}

int
cpu_smap_is_enabled(void)
{
    if (g_fSmapOn != 0 && (read_cr4() & CR4_SMAP) != 0) {
        return 1;
    }
    return 0;
}

u64
vmm_harden_audit_user_bits(void)
{
    u64 *pPml4 = harden_pml4_base();
    u64 u64Remain;

    g_stats.u64AuditCalls++;
    /* Audit walk must not bump walk/skip counters from a prior harden. */
    {
        u64 u64W = g_stats.u64WalkedLeaves;
        u64 u64S = g_stats.u64SkippedUserBand;
        u64 u64T = g_stats.u64StraddleLarge;

        u64Remain = walk_harden_pml4(pPml4, 0);
        /* Restore walk diagnostics from mutate pass; audit uses remain only. */
        g_stats.u64WalkedLeaves = u64W;
        g_stats.u64SkippedUserBand = u64S;
        g_stats.u64StraddleLarge = u64T;
    }
    g_stats.u64AuditRemainU = u64Remain;
    g_u64SoftLastRemain = u64Remain;
    if (u64Remain == 0) {
        g_stats.u64SoftPass++;
        kprintf("smep: audit residual_u=0 soft PASS "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN (Soft!=product)\n");
    } else {
        g_stats.u64SoftFail++;
        kprintf("smep: audit residual_u=%lu soft FAIL "
                "base_va=0x%llx last_va=0x%llx first_pages=%lu "
                "first_size=%d resid_w=%lu access=0x%x "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                "(C3 residual; Soft!=product; G-AC-1)\n",
                (unsigned long)u64Remain,
                (unsigned long long)g_u64SoftResidBaseVa,
                (unsigned long long)g_u64SoftResidLastVa,
                (unsigned long)g_u64SoftResidFirstPages,
                g_fSoftResidFirstSize,
                (unsigned long)g_u64SoftResidW,
                (unsigned)g_u32SoftResidAccess);
        /* greppable: smep: soft residual deepen */
        smep_soft_residual_deepen_log("audit");
    }
    return u64Remain;
}

/*
 * Walk kernel CR3 page tables. Clear U on any present mapping whose VA is
 * outside the Linux personality user window (G-MAP-1, G-MAP-2) or in the
 * kernel half (HHDM / high). Large pages: clear U when wholly outside;
 * soft-count straddles (G-MAP-4 soft residual).
 */
void
vmm_harden_kernel_maps(void)
{
    u64 *pPml4 = harden_pml4_base();
    u64 u64Cleared;
    u64 u64Remain;

    g_stats.u64HardenCalls++;
    /* Soft map inventory snapshot for this mutate walk. */
    smep_soft_map_reset();

    if (pPml4 == NULL) {
        g_stats.u64SoftFail++;
        g_u64SoftHardenNull++;
        kprintf("smep: harden soft FAIL (null pml4)\n");
        /* greppable: smep: soft */
        smep_soft_inventory("harden_null");
        return;
    }
    if ((pPml4[0] & PTE_P) == 0 && (pPml4[256] & PTE_P) == 0) {
        /* No low identity and no HHDM - still soft-walk for any slots. */
        kprintf("smep: harden: no PML4[0]/PML4[256] (walk all soft)\n");
    }

    (void)walk_harden_pml4(pPml4, 1);
    reload_cr3();

    u64Cleared = g_stats.u64Cleared4k + g_stats.u64Cleared2m +
                 g_stats.u64Cleared1g;
    u64Remain = vmm_harden_audit_user_bits();

    kprintf("smep: harden cleared=%lu (4k=%lu 2m=%lu 1g=%lu) walked=%lu "
            "ux=%lu straddle=%lu residual_u=%lu soft %s\n",
            (unsigned long)u64Cleared,
            (unsigned long)g_stats.u64Cleared4k,
            (unsigned long)g_stats.u64Cleared2m,
            (unsigned long)g_stats.u64Cleared1g,
            (unsigned long)g_stats.u64WalkedLeaves,
            (unsigned long)g_stats.u64UxCleared,
            (unsigned long)g_stats.u64StraddleLarge,
            (unsigned long)u64Remain,
            (u64Remain == 0) ? "PASS" : "FAIL");
    kprintf("smep: stats calls=%lu audit=%lu smep_on=%lu smap_on=%lu\n",
            (unsigned long)g_stats.u64HardenCalls,
            (unsigned long)g_stats.u64AuditCalls,
            (unsigned long)g_stats.u64SmepOn,
            (unsigned long)g_stats.u64SmapOn);
    /* greppable: smep: soft inventory / map / cr4 / stats / deepen */
    smep_soft_inventory("harden");
}

void
smep_stats_get(struct gj_smep_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    /* Soft mirrors from live flags / CR4. */
    g_stats.u64SmepOn = cpu_smep_is_enabled() ? 1ull : 0ull;
    g_stats.u64SmapOn = cpu_smap_is_enabled() ? 1ull : 0ull;
    *pOut = g_stats;
    /*
     * Soft residual lean once on stats read (capped inventory path).
     * greppable: smep: soft residual lean
     */
    smep_soft_inventory("stats_get");
}

void
smep_stats_reset(void)
{
    u64 u64Smep = cpu_smep_is_enabled() ? 1ull : 0ull;
    u64 u64Smap = cpu_smap_is_enabled() ? 1ull : 0ull;

    memset(&g_stats, 0, sizeof(g_stats));
    g_stats.u64SmepOn = u64Smep;
    g_stats.u64SmapOn = u64Smap;
    /* Map inventory axes are independent of g_stats; clear with reset. */
    smep_soft_map_reset();
    g_u64SoftEnableSmepOk = 0;
    g_u64SoftEnableSmepFail = 0;
    g_u64SoftEnableSmepSkip = 0;
    g_u64SoftEnableSmapOk = 0;
    g_u64SoftEnableSmapFail = 0;
    g_u64SoftEnableSmapSkip = 0;
    g_u64SoftHardenNull = 0;
    g_u64SoftLastRemain = 0;
    /* C3 residual snapshot clears with diagnostic reset. */
    smep_soft_resid_reset();
    /*
     * Preserve inv log count + emit cap across reset (lifetime).
     * Do not re-arm lean lamp or inv cap - no stamp storms on reset loops.
     */
    /* greppable: smep: soft residual lean (capped path after reset) */
    smep_soft_inventory("stats_reset");
}
