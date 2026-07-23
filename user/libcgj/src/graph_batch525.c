/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch525: largest power of two not exceeding x.
 *
 * Surface (unique symbols):
 *   uint64_t gj_prev_power2(uint64_t x);
 *     — Return the largest power of two that is <= x. If x is already a
 *       power of two, return x. If x == 0, return 0 (no positive power
 *       of two is <= 0). For x in [1, UINT64_MAX], result is 2^floor(log2(x)).
 *       Examples: 0 → 0; 1 → 1; 3 → 2; 7 → 4; 8 → 8; UINT64_MAX → 1<<63.
 *   uint64_t __gj_prev_power2  (alias)
 *   __libcgj_batch525_marker = "libcgj-batch525"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch525_marker[] = "libcgj-batch525";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prev_power2 — floor of x to the nearest (not exceeding) power of two.
 *
 * Zero is defined as 0. Otherwise smear set bits downward through all
 * lower bit positions, then isolate the highest remaining bit via
 * x - (x >> 1). Works for the full uint64_t domain including 1<<63.
 */
uint64_t
gj_prev_power2(uint64_t x)
{
	if (x == 0u) {
		return 0u;
	}

	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;
	return x - (x >> 1);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_prev_power2(uint64_t x)
    __attribute__((alias("gj_prev_power2")));
