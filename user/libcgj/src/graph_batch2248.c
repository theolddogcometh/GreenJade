/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2248: previous (floor) power of two for
 * uint64_t (post-2240 math exclusive wave 2241-2250).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_prev_pow2(uint64_t x);
 *     - Largest power of two <= x. x == 0 -> 0.
 *       Already pow2 -> x. 3 -> 2; 7 -> 4; UINT64_MAX -> 1<<63.
 *   uint64_t __gj_u64_prev_pow2  (alias)
 *   __libcgj_batch2248_marker = "libcgj-batch2248"
 *
 * Distinct from gj_prev_power2 (batch525). Unique gj_u64_prev_pow2
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2248_marker[] = "libcgj-batch2248";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor x to nearest power of two not exceeding x. x must be non-zero.
 * Smear set bits downward, then isolate highest bit via x - (x >> 1).
 */
static uint64_t
b2248_prev_pow2(uint64_t u64X)
{
	u64X |= u64X >> 1;
	u64X |= u64X >> 2;
	u64X |= u64X >> 4;
	u64X |= u64X >> 8;
	u64X |= u64X >> 16;
	u64X |= u64X >> 32;
	return u64X - (u64X >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_prev_pow2 - floor of x to the nearest (not exceeding) power
 * of two.
 *
 * Zero is defined as 0. Otherwise smear set bits downward through all
 * lower bit positions, then isolate the highest remaining bit. Works
 * for the full uint64_t domain including 1<<63.
 */
uint64_t
gj_u64_prev_pow2(uint64_t u64X)
{
	(void)NULL;
	if (u64X == 0ull) {
		return 0ull;
	}
	return b2248_prev_pow2(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_prev_pow2(uint64_t u64X)
    __attribute__((alias("gj_u64_prev_pow2")));
