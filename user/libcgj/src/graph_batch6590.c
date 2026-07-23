/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6590: CPUID feature leaf continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpuid_feat_leaf_continuum_6590(void);
 *     - Returns 1 (soft compile-time product tag: CPUID feature leaf
 *       stub continuum 6581-6590 is complete / ready).
 *   uint32_t gj_batch_id_6590(void);
 *     - Returns the compile-time graph batch number for this TU (6590).
 *   uint32_t __gj_cpuid_feat_leaf_continuum_6590  (alias)
 *   uint32_t __gj_batch_id_6590  (alias)
 *   __libcgj_batch6590_marker = "libcgj-batch6590"
 *
 * Exclusive continuum CREATE-ONLY (6581-6590: CPUID feature leaf
 * stubs —
 * cpuid_leaf_basic_max_6581, cpuid_leaf_std_feat_6582,
 * cpuid_leaf_ext_feat_6583, cpuid_leaf_ok_6584,
 * cpuid_feat_bit_test_6585, cpuid_subleaf_need_6586,
 * cpuid_leaf_xsave_6587, cpuid_leaf_ext_base_6588,
 * cpuid_feat_leaf_stub_ready_6589, continuum + batch_id_6590).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6590_marker[] = "libcgj-batch6590";

/* Continuum-complete lamp for the 6581-6590 exclusive wave. */
#define B6590_CONTINUUM_READY  1u
#define B6590_BATCH_ID         6590u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6590_continuum(void)
{
	return B6590_CONTINUUM_READY;
}

static uint32_t
b6590_id(void)
{
	return B6590_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpuid_feat_leaf_continuum_6590 - continuum-ready tag for 6581-6590.
 *
 * Always returns 1. Soft pure-data product tag that the CPUID feature
 * leaf stub exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_cpuid_feat_leaf_continuum_6590(void)
{
	(void)NULL;
	return b6590_continuum();
}

/*
 * gj_batch_id_6590 - report this TU's graph batch number.
 *
 * Always returns 6590.
 */
uint32_t
gj_batch_id_6590(void)
{
	return b6590_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_cpuid_feat_leaf_continuum_6590(void)
    __attribute__((alias("gj_cpuid_feat_leaf_continuum_6590")));

uint32_t __gj_batch_id_6590(void)
    __attribute__((alias("gj_batch_id_6590")));
