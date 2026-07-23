/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6584: CPUID basic leaf catalog range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpuid_leaf_ok_6584(uint32_t leaf);
 *     - Returns 1 when leaf is in soft basic catalog range [0, 0x20],
 *       else 0. Pure-data range stub; not a live max-leaf compare.
 *   uint32_t __gj_cpuid_leaf_ok_6584  (alias)
 *   __libcgj_batch6584_marker = "libcgj-batch6584"
 *
 * Exclusive continuum CREATE-ONLY (6581-6590: CPUID feature leaf
 * stubs). Unique gj_cpuid_leaf_ok_6584 surface only; no multi-def.
 * Soft bound matches gj_cpuid_leaf_basic_max_6581 (0x20). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6584_marker[] = "libcgj-batch6584";

/* Soft basic catalog ceiling (inclusive). Matches batch6581. */
#define B6584_BASIC_MAX  0x20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6584_leaf_ok(uint32_t uLeaf)
{
	return (uLeaf <= B6584_BASIC_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpuid_leaf_ok_6584 - soft basic CPUID leaf catalog membership.
 *
 * leaf: candidate basic leaf number.
 * Returns 1 if leaf is in [0, 0x20], else 0. Does not execute CPUID.
 * No parent wires.
 */
uint32_t
gj_cpuid_leaf_ok_6584(uint32_t uLeaf)
{
	(void)NULL;
	return b6584_leaf_ok(uLeaf);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cpuid_leaf_ok_6584(uint32_t uLeaf)
    __attribute__((alias("gj_cpuid_leaf_ok_6584")));
