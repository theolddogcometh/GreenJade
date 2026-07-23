/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2725: test a single bit in a uint64_t word (_u).
 *
 * Surface (unique symbols):
 *   int gj_bit_test_u64_u(uint64_t x, unsigned b);
 *     - Return 1 if bit b of x is set (bit 0 = LSB), else 0.
 *       Out-of-range index b >= 64 yields 0 (no shift UB).
 *   int __gj_bit_test_u64_u  (alias)
 *   __libcgj_batch2725_marker = "libcgj-batch2725"
 *
 * Bitops exclusive wave (2721-2730). Distinct from gj_bit_test_u64
 * (batch653), gj_u64_test_bit (batch2014) — unique gj_bit_test_u64_u
 * surface only; no multi-def. Suggested bare gj_bit_test_u64 is already
 * owned by batch653.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2725_marker[] = "libcgj-batch2725";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2725_test(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return 0;
	}
	return ((u64X >> uB) & (uint64_t)1) != (uint64_t)0 ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_test_u64_u - test whether bit b of x is set.
 *
 * x: 64-bit word
 * b: bit index (0 = least significant bit)
 *
 * Returns 1 if (x & (1 << b)) is non-zero and b < 64; returns 0 if
 * b >= 64 or the selected bit is clear.
 */
int
gj_bit_test_u64_u(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b2725_test(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bit_test_u64_u(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_bit_test_u64_u")));
