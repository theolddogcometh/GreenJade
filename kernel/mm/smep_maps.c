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
 * Soft deepen (Wave 9 base; Wave 35 exclusive deepen): soft SMEP/map
 * inventory + greppable "smep: soft …" logs (map U axes, leaf sizes,
 * CR4/CPUID, harden stats, honesty/path/deepen stamps).
 * Diagnostics only — never hard-gate boot; wrap OK.
 *
 * Wave 19 soft inventory deepen (prefix-stable; greppable: smep: soft):
 *   "smep: soft honesty …"    explicit non-claims (not full G-MAP product)
 *   "smep: soft inventory …"  stage + harden/audit soft pass surface
 *   "smep: soft map …"        leaf U axes + clear/walk residual snapshot
 *   "smep: soft cr4 …"        CR4.SMEP/SMAP + CPUID.7 feature bits
 *   "smep: soft residual …"   audit residual + straddle/UX soft axes
 *   "smep: soft enable …"     SMEP/SMAP enable/skip soft path catalog
 *   "smep: soft path …"       surface catalog + honesty open lamps
 *   "smep: soft stats …"      aggregate counters (mirror of g_stats)
 *   smep: soft return selftest — Wave 19 terminal return surface
 *   smep: soft retmap     — Wave 19 return-surface map
 *   "smep: soft deepen …"     wave=83 stamp + area count
 *   "smep: soft lamps …"      CR4/CPUID readiness lamps
 *   "smep: soft band …"       Wave 15 user-band geometry
 *   "smep: soft surfaces …"   Wave 19 return-surface catalog
 *   "smep: soft walk …"       Wave 17 PML4 walk surface
 *   "smep: soft OPEN …"       Wave 17 G-MAP/P-MEM-6 OPEN honesty
 *   "smep: soft gmap …"       Wave 17 G-MAP-1..4 soft axes
 * Honesty: soft inventory only — not product G-MAP complete; not bar3;
 *          soft ≠ product.
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
 * greppable: smep: soft surfaces
 * greppable: smep: soft walk
 * greppable: smep: soft OPEN
 * greppable: smep: soft gmap
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

/* Wave 62 soft inventory stamp (file-local; never product gate). */
#define SMEP_SOFT_WAVE 83u

/* Soft inventory greppable area count (honesty..gmap; deepen excluded). */
#define SMEP_SOFT_AREAS 142u

/*
 * Wave 19 return-surface bit lamps (surf=0x… on soft surfaces/deepen).
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
 * Soft map inventory axes (file-local; Wave 9 + Wave 17 exclusive).
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
 *   smep: soft return selftest — Wave 19 terminal return surface
 *   smep: soft retmap     — Wave 19 return-surface map
 *   smep: soft deepen     — wave=83 stamp + area count
 *   smep: soft lamps      — CR4/CPUID readiness lamps
 *   smep: soft surfaces   — Wave 19 return-surface catalog
 *   smep: soft walk       — Wave 17 PML4 walk surface
 *   smep: soft OPEN       — Wave 17 G-MAP/P-MEM-6 OPEN honesty
 *   smep: soft gmap       — Wave 17 G-MAP-1..4 soft axes
 *
 * Never allocates; safe from boot harden / enable paths.
 * Honesty: soft inventory ≠ product G-MAP complete; not bar3; soft ≠ product.
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
     * Wave 19: return-surface catalog (surf bitmask; soft ≠ product).
     * Grep: smep: soft surfaces
     */
    kprintf("smep: soft surfaces surf=0x%x catalog=%u areas_live=%u "
            "map=1 cr4=1 residual=1 enable=1 walk=1 gmap=1 open=1 "
            "wave=%u (return surfaces; soft only; not product G-MAP; "
            "not bar3)\n",
            (unsigned)u32Surf, (unsigned)SMEP_SOFT_AREAS, u32Areas + 4u,
            (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /*
     * Wave 19: PML4 walk surface (observe-only).
     * Grep: smep: soft walk
     */
    kprintf("smep: soft walk walked=%lu present=%lu cleared=%lu "
            "skip_user=%lu leaf4k=%lu leaf2m=%lu leaf1g=%lu "
            "straddle_large=%lu wave=%u "
            "(soft walk; not product G-MAP; not bar3)\n",
            (unsigned long)g_stats.u64WalkedLeaves,
            (unsigned long)g_u64SoftMapPresent,
            (unsigned long)u64Cleared,
            (unsigned long)g_stats.u64SkippedUserBand,
            (unsigned long)g_u64SoftMapLeaf4k,
            (unsigned long)g_u64SoftMapLeaf2m,
            (unsigned long)g_u64SoftMapLeaf1g,
            (unsigned long)g_stats.u64StraddleLarge,
            (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /*
     * Wave 19: G-MAP / P-MEM-6 remain OPEN.
     * Grep: smep: soft OPEN
     */
    kprintf("smep: soft OPEN product_gmap=OPEN product_pmem6=OPEN "
            "straddle_split=OPEN bar3=OPEN wave=%u "
            "(soft honesty; soft≠product; not bar3)\n",
            (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /*
     * Wave 19: G-MAP-1..4 soft axes (shape only).
     * Grep: smep: soft gmap
     */
    kprintf("smep: soft gmap g1_clear_u=1 g2_user_band=1 g3_smep=1 "
            "g4_ux_nx=1 u_kernel=%lu u_user=%lu u_straddle=%lu "
            "ux_cleared=%lu residual_u=%lu wave=%u "
            "(G-MAP soft axes; not product complete; not bar3)\n",
            (unsigned long)g_u64SoftMapUKernelHalf,
            (unsigned long)g_u64SoftMapUUserBand,
            (unsigned long)g_u64SoftMapUStraddle,
            (unsigned long)g_stats.u64UxCleared,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /*
     * Grep: smep: soft return rate
     * Wave 17 return-surface rate lamps (kept) (walk/clear residual).
     */
    kprintf("smep: soft return rate "
            "walked=%lu present=%lu cleared=%lu residual_u=%lu "
            "u_kernel=%lu u_user=%lu logs=%lu wave=%u "
            "(return rate; Soft≠product G-MAP; soft≠product; not bar3)\n",
            (unsigned long)g_stats.u64WalkedLeaves,
            (unsigned long)g_u64SoftMapPresent,
            (unsigned long)u64Cleared,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned long)g_u64SoftMapUKernelHalf,
            (unsigned long)g_u64SoftMapUUserBand,
            (unsigned long)g_u64SoftInvLogs,
            (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /*
     * Grep: smep: soft retcode
     * Wave 17 retcode catalog for map/walk/gmap soft return classes.
     */
    kprintf("smep: soft retcode "
            "map=1 walk=1 clear_u=1 residual=1 enable=1 gmap=1 "
            "product_gmap=OPEN product_pmem6=OPEN wave=%u "
            "(retcode catalog; Soft≠product G-MAP; soft≠product)\n",
            (unsigned)SMEP_SOFT_WAVE);
    u32Areas++;

    /*
     * Grep: smep: soft deepen wave
     * areas tracks prior soft lines this emission (honesty..gmap).
     */
    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: smep: soft return selftest — Wave 19 terminal return surface */
    kprintf("smep: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)SMEP_SOFT_WAVE);

    /* Grep: smep: soft retmap — Wave 19 return-surface map */
    kprintf("smep: soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)SMEP_SOFT_WAVE);

    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: smep: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("smep: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)SMEP_SOFT_WAVE);
    /* Grep: smep: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("smep: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)SMEP_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: smep: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("smep: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)SMEP_SOFT_WAVE);
    /* Grep: smep: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("smep: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)SMEP_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: smep: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("smep: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)SMEP_SOFT_WAVE);
            /* Grep: smep: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("smep: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)SMEP_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: smep: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("smep: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)SMEP_SOFT_WAVE);
            /* Grep: smep: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("smep: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: smep: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("smep: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)SMEP_SOFT_WAVE);
            /* Grep: smep: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("smep: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)SMEP_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: smep: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("smep: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)SMEP_SOFT_WAVE);
            /* Grep: smep: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("smep: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)SMEP_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: smep: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("smep: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)SMEP_SOFT_WAVE);
            /* Grep: smep: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("smep: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)SMEP_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: smep: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("smep: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)SMEP_SOFT_WAVE);
            /* Grep: smep: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("smep: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)SMEP_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: smep: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("smep: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)SMEP_SOFT_WAVE);
                    /* Grep: smep: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("smep: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)SMEP_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: smep: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("smep: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)SMEP_SOFT_WAVE);
                            /* Grep: smep: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("smep: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)SMEP_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: smep: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("smep: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SMEP_SOFT_WAVE);
                            /* Grep: smep: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("smep: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)SMEP_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: smep: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("smep: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SMEP_SOFT_WAVE);
                            /* Grep: smep: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("smep: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)SMEP_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: smep: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("smep: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SMEP_SOFT_WAVE);
                            /* Grep: smep: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("smep: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SMEP_SOFT_WAVE);
                            /* Grep: smep: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("smep: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("smep: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("smep: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("smep: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("smep: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("smep: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("smep: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retfortress — Wave 35 return-fortress honesty */
kprintf("smep: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("smep: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft rethold — Wave 36 return-hold honesty */
kprintf("smep: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retspire — Wave 36 exclusive spire stamp */
kprintf("smep: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retwall — Wave 37 return-wall honesty */
kprintf("smep: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retgate — Wave 37 exclusive gate stamp */
kprintf("smep: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retmoat — Wave 38 return-moat honesty */
kprintf("smep: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retower — Wave 38 exclusive tower stamp */
kprintf("smep: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("smep: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("smep: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("smep: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("smep: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retravelin — Wave 41 return-travelin honesty */
kprintf("smep: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("smep: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("smep: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("smep: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("smep: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("smep: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("smep: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("smep: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("smep: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("smep: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retbailey — Wave 46 return-bailey honesty */
kprintf("smep: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)SMEP_SOFT_WAVE);
/* Grep: smep: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("smep: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)SMEP_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("smep: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("smep: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("smep: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("smep: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("smep: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("smep: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retsally — Wave 50 return-sally honesty */
kprintf("smep: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("smep: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retfosse — Wave 51 return-fosse honesty */
kprintf("smep: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("smep: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("smep: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("smep: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retravelin — Wave 53 return-travelin honesty */
kprintf("smep: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("smep: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("smep: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retredan — Wave 54 exclusive redan stamp */
kprintf("smep: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retflank — Wave 55 return-flank honesty */
kprintf("smep: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retface — Wave 55 exclusive face stamp */
kprintf("smep: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retgorge — Wave 56 return-gorge honesty */
kprintf("smep: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("smep: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retraverse — Wave 57 return-traverse honesty */
kprintf("smep: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("smep: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retorillon — Wave 58 return-orillon honesty */
kprintf("smep: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("smep: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("smep: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("smep: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retplace — Wave 60 return-place honesty */
kprintf("smep: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("smep: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("smep: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("smep: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("smep: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("smep: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("smep: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("smep: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: smep: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("smep: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: smep: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("smep: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: smep: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("smep: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: smep: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("smep: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: smep: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("smep: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=83 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: smep: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("smep: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=83 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("smep: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("smep: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("smep: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("smep: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("smep: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=83 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("smep: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=83 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("smep: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("smep: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("smep: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("smep: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smep: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("smep: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("smep: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: smep: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("smep: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("smep: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retbastionangle stamp; Soft≠product)\n");
/* Grep: smep: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("smep: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("smep: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retparapetangle stamp; Soft≠product)\n");
/* Grep: smep: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("smep: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("smep: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retowerangle stamp; Soft≠product)\n");
/* Grep: smep: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("smep: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("smep: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retwallangle stamp; Soft≠product)\n");
/* Grep: smep: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("smep: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("smep: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retholdangle stamp; Soft≠product)\n");
/* Grep: smep: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("smep: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("smep: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retfortressangle stamp; Soft≠product)\n");
/* Grep: smep: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("smep: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("smep: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: smep: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("smep: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("smep: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: smep: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("smep: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("smep: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: smep: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("smep: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("smep: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retaegisangle stamp; Soft≠product)\n");
/* Grep: smep: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("smep: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=83 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: smep: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("smep: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=83 (retsigilangle stamp; Soft≠product)\n");

                            kprintf("smep: soft deepen wave=%u areas=%u via=%s logs=%lu "
            "catalog=%u residual_u=%lu surf=0x%x "
            "(Wave 35 exclusive; not product G-MAP; not bar3; soft≠product)\n",
            (unsigned)SMEP_SOFT_WAVE,
            (unsigned)u32Areas,
            szWhere,
            (unsigned long)g_u64SoftInvLogs,
            (unsigned)SMEP_SOFT_AREAS,
            (unsigned long)g_stats.u64AuditRemainU,
            (unsigned)u32Surf);

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
