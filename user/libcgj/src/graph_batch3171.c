/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3171: test one bit in a uint32_t bitset word.
 *
 * Surface (unique symbols):
 *   int gj_bitset32_test_u(uint32_t x, unsigned b);
 *     - Return 1 if bit b of x is set (bit 0 = LSB), else 0.
 *       Out-of-range index b >= 32 yields 0 (no shift UB).
 *   int __gj_bitset32_test_u  (alias)
 *   __libcgj_batch3171_marker = "libcgj-batch3171"
 *
 * Milestone 3180 exclusive continuum CREATE-ONLY (3171-3180). Distinct
 * from gj_bit_test_u64 (batch653) and array bitset test (batch118) —
 * unique gj_bitset32_test_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3171_marker[] = "libcgj-batch3171";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if bit b of x is set and b < 32; else 0. */
static int
b3171_test(uint32_t u32X, unsigned uB)
{
	if (uB >= 32u) {
		return 0;
	}
	return ((u32X >> uB) & 1u) != 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset32_test_u - test whether bit b of a 32-bit bitset word is set.
 *
 * x: 32-bit bitset word
 * b: bit index (0 = least significant bit)
 *
 * Returns 1 if (x & (1u << b)) is non-zero and b < 32; returns 0 if
 * b >= 32 or the selected bit is clear. No parent wires.
 */
int
gj_bitset32_test_u(uint32_t u32X, unsigned uB)
{
	(void)NULL;
	return b3171_test(u32X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset32_test_u(uint32_t u32X, unsigned uB)
    __attribute__((alias("gj_bitset32_test_u")));
