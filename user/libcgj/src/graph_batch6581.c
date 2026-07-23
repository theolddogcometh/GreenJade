/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6581: soft CPUID basic-leaf catalog maximum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpuid_leaf_basic_max_6581(void);
 *     - Returns 0x20 (soft upper bound for basic CPUID leaf numbers
 *       in the product catalog). Pure-data stub; not a live CPUID.0
 *       EAX read.
 *   uint32_t __gj_cpuid_leaf_basic_max_6581  (alias)
 *   __libcgj_batch6581_marker = "libcgj-batch6581"
 *
 * Exclusive continuum CREATE-ONLY (6581-6590: CPUID feature leaf
 * stubs —
 * cpuid_leaf_basic_max_6581, cpuid_leaf_std_feat_6582,
 * cpuid_leaf_ext_feat_6583, cpuid_leaf_ok_6584,
 * cpuid_feat_bit_test_6585, cpuid_subleaf_need_6586,
 * cpuid_leaf_xsave_6587, cpuid_leaf_ext_base_6588,
 * cpuid_feat_leaf_stub_ready_6589, continuum + batch_id_6590).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6581_marker[] = "libcgj-batch6581";

/* Soft catalog bound for basic CPUID leaves (not a live CPUID.0 result). */
#define B6581_BASIC_MAX  0x20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6581_basic_max(void)
{
	return B6581_BASIC_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpuid_leaf_basic_max_6581 - soft max basic CPUID leaf catalog bound.
 *
 * Always returns 0x20. Soft compile-time catalog ceiling for basic
 * feature leaves; does not execute CPUID. No parent wires.
 */
uint32_t
gj_cpuid_leaf_basic_max_6581(void)
{
	(void)NULL;
	return b6581_basic_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cpuid_leaf_basic_max_6581(void)
    __attribute__((alias("gj_cpuid_leaf_basic_max_6581")));
