/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8275: power-of-two predicate for uint32_t.
 *
 * Surface (unique symbols):
 *   int gj_is_pow2_u32_8275(uint32_t n);
 *     - Return 1 if n is a positive power of two (exactly one bit set
 *       in the 32-bit value), else 0. Zero is not a power of two.
 *       Examples: 1, 2, 4, ..., 1<<31 → 1; 0, 3, 6, 10 → 0.
 *   int __gj_is_pow2_u32_8275  (alias)
 *   __libcgj_batch8275_marker = "libcgj-batch8275"
 *
 * Exclusive continuum CREATE-ONLY (8271-8280: div/mod safe integer
 * stubs — div_safe, mod_safe, div_ceil, div_floor, is_pow2, align_up,
 * align_down, round_up_pow2, gcd, batch_id_8280). Unique
 * gj_is_pow2_u32_8275 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8275_marker[] = "libcgj-batch8275";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 iff n is 2^k for some k in [0, 31].
 * Classic bit test: a positive power of two has n & (n-1) == 0.
 * Zero yields 0 (n != 0 guard).
 */
static int
b8275_is_pow2(uint32_t u32N)
{
	if (u32N == 0u) {
		return 0;
	}
	return ((u32N & (u32N - 1u)) == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_pow2_u32_8275 - 1 iff n is 2^k for some k in [0, 31].
 *
 * n: value to test
 * No parent wires.
 */
int
gj_is_pow2_u32_8275(uint32_t u32N)
{
	(void)NULL;
	return b8275_is_pow2(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_pow2_u32_8275(uint32_t u32N)
    __attribute__((alias("gj_is_pow2_u32_8275")));
