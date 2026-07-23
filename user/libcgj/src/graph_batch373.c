/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch373: power-of-two predicate for uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_is_pow2_u64(uint64_t n);
 *     — Return 1 if n is a positive power of two (exactly one bit set
 *       in the 64-bit value), else 0. Zero is not a power of two.
 *       Examples: 1, 2, 4, ..., 1<<63 → 1; 0, 3, 6, 10 → 0.
 *   int __gj_is_pow2_u64  (alias)
 *   __libcgj_batch373_marker = "libcgj-batch373"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch373_marker[] = "libcgj-batch373";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_pow2_u64 — 1 iff n is 2^k for some k in [0, 63].
 *
 * Classic bit test: a positive power of two has n & (n-1) == 0.
 * Zero yields 0 (n != 0 guard).
 */
int
gj_is_pow2_u64(uint64_t u64N)
{
	if (u64N == 0u) {
		return 0;
	}
	return ((u64N & (u64N - 1u)) == 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_pow2_u64(uint64_t u64N)
    __attribute__((alias("gj_is_pow2_u64")));
