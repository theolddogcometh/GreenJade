/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clear USER bit on kernel half; enable SMEP/SMAP (G-MAP-1..4, P-MEM-6).
 *
 * Soft deepen:
 *   - Full PML4 walk: low half outside user band + all kernel-half leaves
 *   - 1GiB / 2MiB / 4K PS leaves; straddle large-page soft residual count
 *   - G-MAP-4 soft: count U+!NX clears as UX residual fixed
 *   - Post-harden residual-U audit → greppable soft PASS/FAIL
 *   - CPUID-gated CR4.SMEP / CR4.SMAP enable + soft query/stats
 *
 * Soft deepen (Wave 9 base; Wave 15 exclusive deepen): soft SMEP/map
 * inventory + greppable "smep: soft …" logs (map U axes, leaf sizes,
 * CR4/CPUID, harden stats, honesty/path/deepen stamps).
 * Diagnostics only — never hard-gate boot; wrap OK.
 *
 * Wave 15 soft inventory deepen (prefix-stable; greppable: smep: soft):
 *   "smep: soft honesty …"    explicit non-claims (not full G-MAP product)
 *   "smep: soft inventory …"  stage + harden/audit soft pass surface
 *   "smep: soft map …"        leaf U axes + clear/walk residual snapshot
 *   "smep: soft cr4 …"        CR4.SMEP/SMAP + CPUID.7 feature bits
 *   "smep: soft residual …"   audit residual + straddle/UX soft axes
 *   "smep: soft enable …"     SMEP/SMAP enable/skip soft path catalog
 *   "smep: soft path …"       surface catalog + honesty open lamps
 *   "smep: soft stats …"      aggregate counters (mirror of g_stats)
 *   "smep: soft deepen …"     wave=15 stamp + area count
 *   "smep: soft lamps …"      CR4/CPUID readiness lamps
 *   "smep: soft band …"       Wave 15 user-band geometry
 * Honesty: soft inventory only — not product G-MAP complete; not bar3.
 *
 * greppable: smep: harden
 * greppable: smep: SMEP
 * greppable: smep: SMAP
 * greppable: smep: audit
 * greppable: smep: stats
 * greppable: smep: soft
 * greppable: smep: soft honesty
 * greppable: smep: soft inventory
 * greppable: smep: soft map
 * greppable: smep: soft cr4
 * greppable: smep: soft residual
 * greppable: smep: soft enable
 * greppable: smep: soft path
 * greppable: smep: soft stats
 * greppable: smep: soft deepen
 * greppable: smep: soft lamps
 * greppable: SMEP_HARDEN_STATS
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/smep.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

#define PTE_P   (1ull << 0)
#define PTE_U   (1ull << 2)
#define PTE_PS  (1ull << 7)
#define PTE_NX  (1ull << 63)
#define PTE_ADDR_MASK 0x000ffffffffff000ull

#define CR4_SMEP (1ull << 20)
#define CR4_SMAP (1ull << 21)

/* CPUID.7:0 — EBX bits for SMEP / SMAP (Intel SDM). */
#define CPUID7_EBX_SMEP (1u << 7)
#define CPUID7_EBX_SMAP (1u << 20)

/* Canonical sign-extend mask for bit 47 (4-level paging). */
#define CANON_SIGN_MASK 0xffff000000000000ull

/* Wave 15 soft inventory stamp (file-local; never product gate). */
#define SMEP_SOFT_WAVE 15u

/* Soft inventory greppable area count (honesty..lamps+band; deepen excluded). */
#define SMEP_SOFT_AREAS 10u

static struct gj_smep_stats g_stats;
static int                  g_fSmepOn;
static int                  g_fSmapOn;

/*
 * Soft map inventory axes (file-local; Wave 9 + Wave 15 exclusive).
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
static u64 g_u64SoftInvLogs;         /* smep_soft_inventory emissions */

/*
 * Wave 15: enable-path soft tallies (file-local; diagnostics only).
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

/* Soft helpers defined after leaf_must_clear_u / CR4/CPUID statics. */
static void smep_soft_map_reset(void);
static void smep_soft_map_note_leaf(u64 u64Entry, u64 u64Va, u64 u64Cb,
                                    int fKernelHalf, int fSize);
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
        /* Overflow → treat as kernel/high; harden soft. */
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
 * Greppable soft SMEP/map inventory dump (product / smoke; Wave 15 deepen).
 * Prefix-stable markers (smep: soft …):
 *   smep: soft honesty    — explicit non-claims
 *   smep: soft inventory  — stage + harden/audit soft pass surface
 *   smep: soft map        — leaf U axes + clear/walk residual snapshot
 *   smep: soft cr4        — CR4.SMEP/SMAP + CPUID.7 feature bits
 *   smep: soft residual   — audit residual + straddle/UX soft axes
 *   smep: soft enable     — SMEP/SMAP enable/skip soft path
 *   smep: soft path       — surface catalog + honesty open lamps
 *   smep: soft stats      — aggregate counters (mirror of g_stats)
 *   smep: soft deepen     — wave=15 stamp + area count
 *   smep: soft lamps      — CR4/CPUID readiness lamps
 *
 * Never allocates; safe from boot harden / enable paths.
 * Honesty: soft inventory ≠ product G-MAP complete; not bar3.
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
    int fSoftPass;

    g_u64SoftInvLogs++;
    if (szWhere == NULL) {
        szWhere = "path";
    }

    u64Cr4 = read_cr4();
    u32Ebx = cpuid7_ebx();
    fSmepBit = ((u64Cr4 & CR4_SMEP) != 0) ? 1 : 0;
    fSmapBit = ((u64Cr4 & CR4_SMAP) != 0) ? 1 : 0;
    fCpuidSmep = ((u32Ebx & CPUID7_EBX_SMEP) != 0) ? 1 : 0;
    fCpuidSmap = ((u32Ebx & CPUID7_EBX_SMAP) != 0) ? 1 : 0;
    u64Cleared = g_stats.u64Cleared4k + g_stats.u64Cleared2m +
                 g_stats.u64Cleared1g;
    u32Areas = 0;

    /*
     * Honesty first: freestanding soft inventory is NOT product G-MAP close.
     * Grep: smep: soft honesty
     */
    kprintf("smep: soft honesty not-product-GMAP not-full-P-MEM-6 "
            "g_map=soft p_mem6=soft product_gmap=OPEN product_pmem6=OPEN "
            "straddle_split=OPEN bar3=OPEN wave=%u "
            "(soft inventory only; never hard-gates boot)\n",
            (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /* Grep: smep: soft inventory */
    kprintf("smep: soft inventory via=%s harden=%lu audit=%lu "
            "pass=%lu fail=%lu residual_u=%lu logs=%lu wave=%u "
            "g_map=1..4 p_mem6=smep+smap user_band=[0x%llx,0x%llx) "
            "(soft; not product G-MAP; not bar3)\n",
            szWhere,
            (unsigned long)g_stats.u64HardenCalls,
            (unsigned long)g_stats.u64AuditCalls,
            (unsigned long)g_stats.u64SoftPass,
            (unsigned long)g_stats.u64SoftFail,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned long)g_u64SoftInvLogs,
            (unsigned)SMEP_SOFT_WAVE,
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

    /* Grep: smep: soft residual */
    kprintf("smep: soft residual last_remain=%lu audit_remain=%lu "
            "straddle_large=%lu ux_cleared=%lu skip_user=%lu "
            "harden_null=%lu soft_pass=%lu soft_fail=%lu "
            "(audit residual; soft only; not product gate)\n",
            (unsigned long)g_u64SoftLastRemain,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned long)g_stats.u64StraddleLarge,
            (unsigned long)g_stats.u64UxCleared,
            (unsigned long)g_stats.u64SkippedUserBand,
            (unsigned long)g_u64SoftHardenNull,
            (unsigned long)g_stats.u64SoftPass,
            (unsigned long)g_stats.u64SoftFail);
    u32Areas++;

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
            "harden_walk→clear_U→reload_cr3→audit→enable_smep→enable_smap "
            "leaf=4k|2m|1g g_map=1..4 p_mem6=soft "
            "straddle_split=OPEN product_gmap=OPEN bar3=OPEN wave=%u "
            "(soft inventory; not bar3)\n",
            szWhere, (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /* Grep: smep: soft stats */
    kprintf("smep: soft stats harden=%lu audit=%lu cleared=%lu "
            "walked=%lu ux=%lu residual_u=%lu soft_pass=%lu soft_fail=%lu "
            "smep_on=%lu smap_on=%lu inv_logs=%lu wave=%u\n",
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
            (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /* Grep: smep: soft lamps */
    kprintf("smep: soft lamps smep_bit=%d smap_bit=%d "
            "cpuid7_smep=%d cpuid7_smap=%d smep_on=%d smap_on=%d "
            "harden_calls=%lu residual_u=%lu wave=%u "
            "(soft readiness; not product gate)\n",
            fSmepBit, fSmapBit, fCpuidSmep, fCpuidSmap,
            g_fSmepOn, g_fSmapOn,
            (unsigned long)g_stats.u64HardenCalls,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /* Grep: smep: soft band (Wave 15 user-band geometry) */
    kprintf("smep: soft band user_base=0x%llx user_end=0x%llx "
            "cr4_smep_bit=%d cr4_smap_bit=%d leaf=4k|2m|1g "
            "u_kernel=%lu u_user=%lu u_straddle=%lu wave=%u\n",
            (unsigned long long)GJ_USER_VA_BASE,
            (unsigned long long)GJ_USER_VA_END,
            fSmepBit, fSmapBit,
            (unsigned long)g_u64SoftMapUKernelHalf,
            (unsigned long)g_u64SoftMapUUserBand,
            (unsigned long)g_u64SoftMapUStraddle,
            (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /*
     * Grep: smep: soft deepen wave
     * areas tracks prior soft lines this emission (honesty..lamps+band).
     */
    kprintf("smep: soft deepen wave=%u areas=%u via=%s logs=%lu "
            "catalog=%u residual_u=%lu "
            "(Wave 15 exclusive; not product G-MAP; not bar3)\n",
            (unsigned)SMEP_SOFT_WAVE,
            (unsigned)u32Areas,
            szWhere,
            (unsigned long)g_u64SoftInvLogs,
            (unsigned)SMEP_SOFT_AREAS,
            (unsigned long)g_stats.u64AuditRemainU);

    /*
     * Soft close lamp: residual U == 0 after any audit is soft-pass shape.
     * Never hard-gates. Grep: smep: soft PASS | smep: soft FAIL
     */
    fSoftPass = (g_stats.u64AuditCalls != 0 &&
                 g_stats.u64AuditRemainU == 0) ? 1 : 0;
    if (fSoftPass != 0) {
        kprintf("smep: soft PASS via=%s residual_u=0 harden=%lu "
                "wave=%u (soft inventory only; not product gate)\n",
                szWhere,
                (unsigned long)g_stats.u64HardenCalls,
                (unsigned)SMEP_SOFT_WAVE);
    } else if (g_stats.u64AuditCalls != 0) {
        kprintf("smep: soft FAIL via=%s residual_u=%lu "
                "wave=%u (soft inventory only; not product gate)\n",
                szWhere,
                (unsigned long)g_stats.u64AuditRemainU,
                (unsigned)SMEP_SOFT_WAVE);
    }

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
 * fMutate 0 → count residual U only (soft audit); 1 → clear.
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
                    }
                } else if (fStraddle != 0) {
                    g_stats.u64StraddleLarge++;
                    /* Soft residual: cannot split without vmm. */
                    if ((pPdpt[u32I3] & PTE_U) != 0) {
                        u64Remain++;
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
                        }
                    } else if (fStraddle != 0) {
                        g_stats.u64StraddleLarge++;
                        if ((pPd[u32I2] & PTE_U) != 0) {
                            u64Remain++;
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
                        }
                    } else if (fStraddle != 0) {
                        /* 4K cannot straddle user edge if PAGE_SIZE aligned. */
                        g_stats.u64StraddleLarge++;
                        if ((pPt[u32I1] & PTE_U) != 0) {
                            u64Remain++;
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
    /* Default AC clear — kernel must STAC before user access */
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
        kprintf("smep: audit residual_u=0 soft PASS\n");
    } else {
        g_stats.u64SoftFail++;
        kprintf("smep: audit residual_u=%lu soft FAIL\n",
                (unsigned long)u64Remain);
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
    /* Wave 9/14: soft map inventory snapshot for this mutate walk. */
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
        /* No low identity and no HHDM — still soft-walk for any slots. */
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
     * Emit soft inventory on stats read so bring-up smoke that
     * snapshots SMEP_HARDEN_STATS also greps smep: soft lines.
     * greppable: smep: soft
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
    /* Preserve inv log count across reset (emission lifetime) — Wave 15:
     * match user_copy; do not zero g_u64SoftInvLogs so logs stay monotonic.
     * Prior Wave 9 zeroed it; deepen prefers lifetime logs like user_copy.
     */
    /* greppable: smep: soft (zeroed inventory after reset) */
    smep_soft_inventory("stats_reset");
}
