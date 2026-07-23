/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6582: CPUID standard feature leaf id (leaf 1).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpuid_leaf_std_feat_6582(void);
 *     - Returns 1 (CPUID leaf 1: processor info and feature bits).
 *       Pure-data leaf id stub; not a live CPUID execution.
 *   uint32_t __gj_cpuid_leaf_std_feat_6582  (alias)
 *   __libcgj_batch6582_marker = "libcgj-batch6582"
 *
 * Exclusive continuum CREATE-ONLY (6581-6590: CPUID feature leaf
 * stubs). Unique gj_cpuid_leaf_std_feat_6582 surface only; no multi-def.
 * Distinct from structured extended features leaf (batch6583).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6582_marker[] = "libcgj-batch6582";

/* CPUID.1 — standard feature information leaf. */
#define B6582_LEAF_STD_FEAT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6582_std_feat(void)
{
	return B6582_LEAF_STD_FEAT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpuid_leaf_std_feat_6582 - standard feature leaf number (CPUID.1).
 *
 * Always returns 1. Soft pure-data leaf id for product feature
 * catalogs; does not execute CPUID. No parent wires.
 */
uint32_t
gj_cpuid_leaf_std_feat_6582(void)
{
	(void)NULL;
	return b6582_std_feat();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cpuid_leaf_std_feat_6582(void)
    __attribute__((alias("gj_cpuid_leaf_std_feat_6582")));
