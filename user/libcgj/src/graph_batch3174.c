/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3174: test one bit in a uint64_t bitset word.
 *
 * Surface (unique symbols):
 *   int gj_bitset64_test_u(uint64_t x, unsigned b);
 *     - Return 1 if bit b of x is set (bit 0 = LSB), else 0.
 *       Out-of-range index b >= 64 yields 0 (no shift UB).
 *   int __gj_bitset64_test_u  (alias)
 *   __libcgj_batch3174_marker = "libcgj-batch3174"
 *
 * Milestone 3180 exclusive continuum CREATE-ONLY (3171-3180). Distinct
 * from gj_bit_test_u64 (batch653) and gj_bitset32_test_u (batch3171) —
 * unique gj_bitset64_test_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3174_marker[] = "libcgj-batch3174";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if bit b of x is set and b < 64; else 0. */
static int
b3174_test(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return 0;
	}
	return ((u64X >> uB) & 1ull) != 0ull ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_test_u - test whether bit b of a 64-bit bitset word is set.
 *
 * x: 64-bit bitset word
 * b: bit index (0 = least significant bit)
 *
 * Returns 1 if (x & (1ull << b)) is non-zero and b < 64; returns 0 if
 * b >= 64 or the selected bit is clear. No parent wires.
 */
int
gj_bitset64_test_u(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b3174_test(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset64_test_u(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_bitset64_test_u")));
