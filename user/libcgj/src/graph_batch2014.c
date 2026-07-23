/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2014: test one bit in a uint64_t word.
 *
 * Surface (unique symbols):
 *   int gj_u64_test_bit(uint64_t x, unsigned b);
 *     - Return 1 if bit b of x is set (bit 0 = LSB), else 0.
 *       Out-of-range index b >= 64 yields 0 (no shift UB).
 *   int __gj_u64_test_bit  (alias)
 *   __libcgj_batch2014_marker = "libcgj-batch2014"
 *
 * Post-2000 bit-ops exclusive wave (2011-2020). Distinct from
 * gj_bit_test_u64 (batch653) - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2014_marker[] = "libcgj-batch2014";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2014_test(uint64_t u64X, unsigned uB)
{
	if (uB >= 64u) {
		return 0;
	}
	return ((u64X >> uB) & (uint64_t)1) != (uint64_t)0 ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_test_bit - test whether bit b of x is set.
 *
 * x: 64-bit word
 * b: bit index (0 = least significant bit)
 *
 * Returns 1 if (x & (1 << b)) is non-zero and b < 64; returns 0 if
 * b >= 64 or the selected bit is clear.
 */
int
gj_u64_test_bit(uint64_t u64X, unsigned uB)
{
	(void)NULL;
	return b2014_test(u64X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_test_bit(uint64_t u64X, unsigned uB)
    __attribute__((alias("gj_u64_test_bit")));
