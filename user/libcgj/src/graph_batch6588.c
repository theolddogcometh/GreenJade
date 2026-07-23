/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6588: CPUID extended-function leaf base.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpuid_leaf_ext_base_6588(void);
 *     - Returns 0x80000000 (first extended-function CPUID leaf).
 *       Pure-data leaf base stub; not a live CPUID.80000000h read.
 *   uint32_t __gj_cpuid_leaf_ext_base_6588  (alias)
 *   __libcgj_batch6588_marker = "libcgj-batch6588"
 *
 * Exclusive continuum CREATE-ONLY (6581-6590: CPUID feature leaf
 * stubs). Unique gj_cpuid_leaf_ext_base_6588 surface only; no multi-def.
 * Distinct from basic catalog max (batch6581). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6588_marker[] = "libcgj-batch6588";

/* CPUID.80000000h — max extended leaf / vendor string path. */
#define B6588_EXT_BASE  0x80000000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6588_ext_base(void)
{
	return B6588_EXT_BASE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpuid_leaf_ext_base_6588 - extended CPUID leaf base (0x80000000).
 *
 * Always returns 0x80000000. Soft pure-data base; does not execute
 * CPUID. No parent wires.
 */
uint32_t
gj_cpuid_leaf_ext_base_6588(void)
{
	(void)NULL;
	return b6588_ext_base();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cpuid_leaf_ext_base_6588(void)
    __attribute__((alias("gj_cpuid_leaf_ext_base_6588")));
