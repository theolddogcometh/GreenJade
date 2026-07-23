/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Supervisor-only kernel maps, SMEP, SMAP (with user_copy STAC/CLAC).
 *
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0.
 * Implementation: kernel/mm/smep_maps.c (+ arch CR4/CPUID in cpu paths).
 *
 * Scope
 * -----
 * Before product ring-3: clear U on kernel-only leaves (G-MAP-1..4), then
 * enable SMEP (and SMAP when copy paths are STAC/CLAC ready). Soft deepen
 * walks full PML4 (low + kernel half), 1G/2M/4K clear, residual-U audit,
 * CPUID-gated enable, greppable soft PASS/stats.
 *
 * Design anchors
 * --------------
 *   docs/DESIGN_SPEC_COMPLETE.md     G-MAP-1..4
 *   docs/X86_64_INTEL_PLATFORM.md    P-MEM-6 (NX, SMEP, SMAP, W^X)
 *   docs/SECURITY_CORE_DESIGN.md     ring-3 cannot exec kernel; fail closed
 *   docs/IMPLEMENTATION.md           harden before user entry
 *
 * G-MAP summary (normative)
 * -------------------------
 *   G-MAP-1  Kernel text/data/rodata/bss/heap maps: U=0
 *   G-MAP-2  User U=1 only in [GJ_USER_VA_BASE, GJ_USER_VA_END)
 *   G-MAP-3  Audit + clear U; enable SMEP; SMAP with STAC/CLAC copy
 *   G-MAP-4  Boot 2 MiB splits must not leave kernel pages user-executable
 *
 * Enable order (product boot)
 * ---------------------------
 *   1. vmm_init / HHDM / product maps as needed
 *   2. vmm_harden_kernel_maps   — clear U outside user band (+ kernel half)
 *   3. vmm_harden_audit_user_bits — soft residual U == 0 ⇒ soft PASS
 *   4. cpu_enable_smep          — CR4.SMEP if CPUID leaf 7
 *   5. cpu_enable_smap          — CR4.SMAP + CLAC; notifies user_access
 *
 * Soft product surface
 * --------------------
 *   SMEP_HARDEN         — full walk clear U on non-user leaves
 *   SMEP_AUDIT          — residual U count (no mutate)
 *   SMEP_ENABLE         — CR4.SMEP CPUID-gated
 *   SMAP_ENABLE         — CR4.SMAP + user_access_smap_enabled
 *   SMEP_HARDEN_STATS   — gj_smep_stats snapshot/reset
 *
 * Layering
 * --------
 *   Walks active/kernel CR3 page tables (vmm geometry / HHDM high)
 *   user_access.h consumes SMAP notify for STAC/CLAC
 *   memobj product maps re-introduce U only inside user VA band
 *
 * greppable: smep: harden
 * greppable: smep: SMEP
 * greppable: smep: SMAP
 * greppable: smep: audit
 * greppable: smep: stats
 * greppable: SMEP_HARDEN_STATS G-MAP P-MEM-6
 */
#pragma once

#include <gj/types.h>

/**
 * Soft product counters for map harden + SMEP/SMAP enable (diagnostics).
 * Wrap OK; never hard-gate boot. Snapshot via smep_stats_get.
 *
 * Cleared*     — U bits stripped by harden (by leaf size)
 * WalkedLeaves — present leaf PTEs/PDEs/PDPTEs visited
 * SkippedUserBand — wholly inside user window left alone (G-MAP-2)
 * StraddleLarge   — large pages partially overlapping user band (soft care)
 * UxCleared       — U cleared on executable (!NX) kernel leaves (G-MAP-4)
 * AuditRemainU    — residual U outside user after last audit
 * SoftPass/Fail   — harden audit residual U == 0 / != 0
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

/**
 * Set CR4.SMEP when CPUID leaf 7 reports SMEP — ring-3 cannot exec U=0.
 * Soft skip + stats if feature missing. greppable: smep: SMEP
 */
void cpu_enable_smep(void);

/**
 * Set CR4.SMAP + CLAC when CPUID reports SMAP; notifies user_copy STAC/CLAC
 * via user_access_smap_enabled(). greppable: smep: SMAP
 */
void cpu_enable_smap(void);

/**
 * Soft query: non-zero if CR4.SMEP is set (and enable succeeded).
 * Mirror may track last enable; product can re-read CR4 in impl.
 */
int cpu_smep_is_enabled(void);

/**
 * Soft query: non-zero if CR4.SMAP is set (and enable succeeded).
 * Pair with user_access_smap_is_enabled for copy-path arming state.
 */
int cpu_smap_is_enabled(void);

/**
 * Clear U on kernel-only maps under active/kernel CR3 (G-MAP-1..4).
 *
 * Walks low half outside [GJ_USER_VA_BASE, GJ_USER_VA_END) and all kernel
 * half leaves (HHDM / high). Soft-audits residual U; greppable soft PASS.
 * Call on kernel CR3 before enabling SMEP for product ring-3.
 * greppable: smep: harden
 */
void vmm_harden_kernel_maps(void);

/**
 * Soft re-audit: count residual U=1 leaves outside the user VA window.
 * Does not mutate PTEs. Returns residual count (0 = clean / soft PASS).
 * greppable: smep: audit
 */
u64 vmm_harden_audit_user_bits(void);

/**
 * Snapshot soft counters into *pOut (no-op if pOut NULL).
 * greppable: smep: stats
 */
void smep_stats_get(struct gj_smep_stats *pOut);

/**
 * Clear soft counters; preserve live SMEP/SMAP on mirrors from CR4 so
 * enable queries stay consistent after diagnostic reset.
 */
void smep_stats_reset(void);
