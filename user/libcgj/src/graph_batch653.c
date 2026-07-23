/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch653: test a single bit in a uint64_t word.
 *
 * Surface (unique symbols):
 *   int gj_bit_test_u64(uint64_t x, unsigned b);
 *     — Return 1 if bit b of x is set (bit 0 = LSB), else 0.
 *       Out-of-range index b >= 64 yields 0 (no shift UB).
 *   int __gj_bit_test_u64  (alias)
 *   __libcgj_batch653_marker = "libcgj-batch653"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch653_marker[] = "libcgj-batch653";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_test_u64 — test whether bit b of x is set.
 *
 * x: 64-bit word
 * b: bit index (0 = least significant bit)
 *
 * Returns 1 if (x & (1 << b)) is non-zero and b < 64; returns 0 if
 * b >= 64 or the selected bit is clear.
 */
int
gj_bit_test_u64(uint64_t x, unsigned b)
{
	if (b >= 64u) {
		return 0;
	}
	return ((x >> b) & 1u) != 0u ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bit_test_u64(uint64_t x, unsigned b)
    __attribute__((alias("gj_bit_test_u64")));
