/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2019: test one bit in a uint32_t word.
 *
 * Surface (unique symbols):
 *   int gj_u32_test_bit(uint32_t x, unsigned b);
 *     - Return 1 if bit b of x is set (bit 0 = LSB), else 0.
 *       Out-of-range index b >= 32 yields 0 (no shift UB).
 *   int __gj_u32_test_bit  (alias)
 *   __libcgj_batch2019_marker = "libcgj-batch2019"
 *
 * Post-2000 bit-ops exclusive wave (2011-2020). Distinct from
 * gj_u64_test_bit (batch2014), gj_bit_test_u64 (batch653), and
 * gj_feature_test_bit (batch1185).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2019_marker[] = "libcgj-batch2019";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2019_test(uint32_t u32X, unsigned uB)
{
	if (uB >= 32u) {
		return 0;
	}
	return ((u32X >> uB) & 1u) != 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_test_bit - test whether bit b of x is set.
 *
 * x: 32-bit word
 * b: bit index (0 = least significant bit)
 *
 * Returns 1 if (x & (1 << b)) is non-zero and b < 32; returns 0 if
 * b >= 32 or the selected bit is clear.
 */
int
gj_u32_test_bit(uint32_t u32X, unsigned uB)
{
	(void)NULL;
	return b2019_test(u32X, uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_test_bit(uint32_t u32X, unsigned uB)
    __attribute__((alias("gj_u32_test_bit")));
