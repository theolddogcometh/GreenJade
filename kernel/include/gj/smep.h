/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * DESIGN_SPEC_COMPLETE G-MAP-* · X86_64_INTEL_PLATFORM P-MEM-6
 * Supervisor-only kernel maps, SMEP, SMAP (with user_copy STAC/CLAC).
 *
 * Soft deepen: full PML4 walk (low + kernel half), 1G/2M/4K clear,
 * residual-U audit, CPUID-gated enable, greppable soft PASS/stats.
 * Pure C freestanding; dual MIT OR Apache-2.0.
 *
 * greppable: smep: harden
 * greppable: smep: SMEP
 * greppable: smep: SMAP
 * greppable: smep: audit
 * greppable: smep: stats
 */
#pragma once

#include <gj/types.h>

/**
 * Soft product counters for map harden + SMEP/SMAP enable (diagnostics).
 * Wrap OK; never hard-gate boot. Snapshot via smep_stats_get.
 *
 * greppable: SMEP_HARDEN_STATS
 */
struct gj_smep_stats {
    u64 u64Cleared4k;       /* 4K leaves U cleared outside user band */
    u64 u64Cleared2m;       /* 2MiB PS leaves U cleared */
    u64 u64Cleared1g;       /* 1GiB PS leaves U cleared */
    u64 u64WalkedLeaves;    /* present leaf PTEs/PDEs/PDPTEs visited */
    u64 u64SkippedUserBand; /* leaves wholly inside user window left alone */
    u64 u64StraddleLarge;   /* large pages partially overlapping user band */
    u64 u64UxCleared;       /* U cleared on executable (!NX) kernel leaves */
    u64 u64AuditRemainU;    /* residual U outside user after last audit */
    u64 u64HardenCalls;     /* vmm_harden_kernel_maps invocations */
    u64 u64AuditCalls;      /* soft audit invocations */
    u64 u64SmepOn;          /* CR4.SMEP live (0/1 soft mirror) */
    u64 u64SmapOn;          /* CR4.SMAP live (0/1 soft mirror) */
    u64 u64SmepSkip;        /* CPUID: SMEP unavailable, enable skipped */
    u64 u64SmapSkip;        /* CPUID: SMAP unavailable, enable skipped */
    u64 u64SoftPass;        /* harden audit residual U == 0 */
    u64 u64SoftFail;        /* harden audit residual U != 0 */
};

/** Set CR4.SMEP when CPUID leaf 7 reports SMEP — ring-3 cannot exec U=0. */
void cpu_enable_smep(void);

/**
 * Set CR4.SMAP + CLAC when CPUID reports SMAP; notifies user_copy STAC/CLAC.
 */
void cpu_enable_smap(void);

/** Soft query: non-zero if CR4.SMEP is set (and enable succeeded). */
int cpu_smep_is_enabled(void);

/** Soft query: non-zero if CR4.SMAP is set (and enable succeeded). */
int cpu_smap_is_enabled(void);

/**
 * Clear U on kernel-only maps under active/kernel CR3 (G-MAP-1..4).
 * Walks low half outside [GJ_USER_VA_BASE, GJ_USER_VA_END) and all kernel
 * half leaves (HHDM / high). Soft-audits residual U; greppable soft PASS.
 * Call on kernel CR3 before enabling SMEP for product ring-3.
 */
void vmm_harden_kernel_maps(void);

/**
 * Soft re-audit: count residual U=1 leaves outside the user VA window.
 * Does not mutate PTEs. Returns residual count (0 = clean / soft PASS).
 * greppable: smep: audit
 */
u64 vmm_harden_audit_user_bits(void);

/** Snapshot soft counters into *pOut (no-op if pOut NULL). */
void smep_stats_get(struct gj_smep_stats *pOut);

/**
 * Clear soft counters; preserve live SMEP/SMAP on mirrors from CR4.
 */
void smep_stats_reset(void);
