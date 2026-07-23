/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2721: test a single bit in a uint32_t word.
 *
 * Surface (unique symbols):
 *   int gj_bit_test_u32(uint32_t x, unsigned b);
 *     - Return 1 if bit b of x is set (bit 0 = LSB), else 0.
 *       Out-of-range index b >= 32 yields 0 (no shift UB).
 *   int __gj_bit_test_u32  (alias)
 *   __libcgj_batch2721_marker = "libcgj-batch2721"
 *
 * Bitops exclusive wave (2721-2730). Distinct from gj_u32_test_bit
 * (batch2019), gj_bit_test_u64 (batch653) — unique gj_bit_test_u32
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2721_marker[] = "libcgj-batch2721";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2721_test(uint32_t u32X, unsigned uB)
{
	if (uB >= 32u) {
		return 0;
	}
	return ((u32X >> uB) & 1u) != 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_test_u32 - test whether bit b of x is set.
 *
 * x: 32-bit word
 * b: bit index (0 = least significant bit)
 *
 * Returns 1 if (x & (1 << b)) is non-zero and b < 32; returns 0 if
 * b >= 32 or the selected bit is clear.
 */
int
gj_bit_test_u32(uint32_t u32X, unsigned uB)
{
	(void)NULL;
	return b2721_test(u32X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bit_test_u32(uint32_t u32X, unsigned uB)
    __attribute__((alias("gj_bit_test_u32")));
