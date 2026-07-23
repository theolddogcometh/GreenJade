/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2246: next power of two for uint64_t
 * (post-2240 math exclusive wave 2241-2250).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_next_pow2(uint64_t x);
 *     - Smallest power of two >= x. x == 0 -> 1.
 *       If x > 0x8000000000000000 (no representable power of two >= x
 *       in uint64_t), returns 0 (overflow: 2^64 not representable).
 *   uint64_t __gj_u64_next_pow2  (alias)
 *   __libcgj_batch2246_marker = "libcgj-batch2246"
 *
 * Distinct from gj_next_pow2_u64 (batch374) / gj_next_power2
 * (batch524). Unique gj_u64_next_pow2 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2246_marker[] = "libcgj-batch2246";

/* Highest representable power of two in uint64_t. */
#define B2246_POW2_MAX 0x8000000000000000ULL

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round x up to next power of two. x in [1, B2246_POW2_MAX]; caller
 * guarantees domain. Already-pow2 values round to themselves via the
 * classic (x-1) smear +1 path.
 */
static uint64_t
b2246_next_pow2(uint64_t u64X)
{
	uint64_t u64V;

	u64V = u64X - 1ull;
	u64V |= u64V >> 1;
	u64V |= u64V >> 2;
	u64V |= u64V >> 4;
	u64V |= u64V >> 8;
	u64V |= u64V >> 16;
	u64V |= u64V >> 32;
	u64V += 1ull;
	return u64V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_next_pow2 - ceil to power of two; 0 on overflow past 2^63.
 *
 * x: input
 *   0            -> 1
 *   already pow2 -> x
 *   > 2^63       -> 0 (2^64 not representable in uint64_t)
 */
uint64_t
gj_u64_next_pow2(uint64_t u64X)
{
	(void)NULL;
	if (u64X == 0ull) {
		return 1ull;
	}
	if (u64X > B2246_POW2_MAX) {
		return 0ull;
	}
	/* Already a power of two? */
	if ((u64X & (u64X - 1ull)) == 0ull) {
		return u64X;
	}
	return b2246_next_pow2(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_next_pow2(uint64_t u64X)
    __attribute__((alias("gj_u64_next_pow2")));
