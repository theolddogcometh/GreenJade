/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6583: CPUID structured extended feature leaf id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpuid_leaf_ext_feat_6583(void);
 *     - Returns 7 (CPUID leaf 7: structured extended feature flags).
 *       Pure-data leaf id stub; not a live CPUID execution.
 *   uint32_t __gj_cpuid_leaf_ext_feat_6583  (alias)
 *   __libcgj_batch6583_marker = "libcgj-batch6583"
 *
 * Exclusive continuum CREATE-ONLY (6581-6590: CPUID feature leaf
 * stubs). Unique gj_cpuid_leaf_ext_feat_6583 surface only; no multi-def.
 * Distinct from standard feature leaf (batch6582). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6583_marker[] = "libcgj-batch6583";

/* CPUID.7 — structured extended feature flags (ECX = subleaf). */
#define B6583_LEAF_EXT_FEAT  7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6583_ext_feat(void)
{
	return B6583_LEAF_EXT_FEAT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpuid_leaf_ext_feat_6583 - extended feature leaf number (CPUID.7).
 *
 * Always returns 7. Soft pure-data leaf id; does not execute CPUID.
 * No parent wires.
 */
uint32_t
gj_cpuid_leaf_ext_feat_6583(void)
{
	(void)NULL;
	return b6583_ext_feat();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cpuid_leaf_ext_feat_6583(void)
    __attribute__((alias("gj_cpuid_leaf_ext_feat_6583")));
