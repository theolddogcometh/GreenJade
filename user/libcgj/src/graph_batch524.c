/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch524: next power of two (uint64_t).
 *
 * Surface (unique symbols):
 *   uint64_t gj_next_power2(uint64_t x);
 *     — Smallest power of two >= x.
 *       x == 0          → 1
 *       x already 2^k   → x
 *       x > 2^63        → 0  (next power would be 2^64, overflow)
 *   uint64_t __gj_next_power2  (alias)
 *   __libcgj_batch524_marker = "libcgj-batch524"
 *
 * Clean-room freestanding pure C (integer only). No compiler builtins
 * (__builtin_clzll etc.). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied.
 */

#include <stdint.h>

const char __libcgj_batch524_marker[] = "libcgj-batch524";

/* Largest power of two representable in uint64_t: 2^63. */
#define B524_POW2_63 ((uint64_t)1 << 63)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_next_power2 — ceil to power of two, or 0 on overflow past 2^63.
 *
 * Uses the classic smear-bits sequence (no clz builtin). Decrement first
 * so that an exact power of two is preserved after the round-trip.
 */
uint64_t
gj_next_power2(uint64_t x)
{
	if (x == 0u) {
		return 1u;
	}
	/* Next power would be 2^64 and does not fit in uint64_t. */
	if (x > B524_POW2_63) {
		return 0u;
	}

	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;
	x++;
	return x;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_next_power2(uint64_t x)
    __attribute__((alias("gj_next_power2")));
