/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4263: freestanding uint64 is-aligned (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_is_aligned_u2(uint64_t x, uint64_t a);
 *     - Return 1 if x is a multiple of a when a is a non-zero power of
 *       two (x & (a - 1) == 0). Return 0 if a == 0. If a is not a
 *       power of two, treat as a = 1 (every x is aligned to 1 → 1).
 *   int __gj_u64_is_aligned_u2  (alias)
 *   __libcgj_batch4263_marker = "libcgj-batch4263"
 *
 * Exclusive continuum CREATE-ONLY (4261-4270): align/page wave.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4263_marker[] = "libcgj-batch4263";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 when x is aligned to a (pow2); a == 0 → 0; non-pow2 → a=1 → 1.
 */
static int
b4263_is_aligned(uint64_t u64X, uint64_t u64A)
{
	if (u64A == 0ull) {
		return 0;
	}

	/* Non-power-of-two → treat as a = 1 (always aligned). */
	if ((u64A & (u64A - 1ull)) != 0ull) {
		return 1;
	}

	return ((u64X & (u64A - 1ull)) == 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_is_aligned_u2 - true (1) when x is aligned to a.
 *
 * a == 0 → 0; non-pow2 treated as a=1 → 1; else mask test.
 * No parent wires.
 */
int
gj_u64_is_aligned_u2(uint64_t u64X, uint64_t u64A)
{
	(void)NULL;
	return b4263_is_aligned(u64X, u64A);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_is_aligned_u2(uint64_t u64X, uint64_t u64A)
    __attribute__((alias("gj_u64_is_aligned_u2")));
