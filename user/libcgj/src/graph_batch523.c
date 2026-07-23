/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch523: power-of-two predicate for uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_is_power2(uint64_t x);
 *     — Return 1 if x is a positive power of two (exactly one bit set
 *       in the 64-bit value) and x != 0, else 0.
 *       Uses the classic test (x & (x - 1)) == 0 with a nonzero guard.
 *       Examples: 1, 2, 4, ..., 1<<63 → 1; 0, 3, 6, 10 → 0.
 *   int __gj_is_power2  (alias)
 *   __libcgj_batch523_marker = "libcgj-batch523"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch523_marker[] = "libcgj-batch523";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_power2 — 1 iff x is 2^k for some k in [0, 63].
 *
 * Classic bit test: a positive power of two has (x & (x - 1)) == 0.
 * Zero yields 0 (x != 0 guard).
 */
int
gj_is_power2(uint64_t x)
{
	if (x == 0u) {
		return 0;
	}
	return ((x & (x - 1u)) == 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_power2(uint64_t x)
    __attribute__((alias("gj_is_power2")));
