/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2913: uint64_t round up to power-of-two
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_round_up_pow2_u(uint64_t n);
 *     - Smallest power of two >= n.
 *       n == 0 -> 1.
 *       If n is already a power of two, return n.
 *       If result would exceed 2^63 (n > 0x8000000000000000), return 0
 *       (overflow: 2^64 not representable in uint64_t).
 *   uint64_t __gj_u64_round_up_pow2_u  (alias)
 *   __libcgj_batch2913_marker = "libcgj-batch2913"
 *
 * Exclusive continuum wave 2911-2920. Distinct from gj_next_pow2_u64
 * (batch374) by exclusive _u name. Unique surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2913_marker[] = "libcgj-batch2913";

/* Highest representable power of two in uint64_t. */
#define B2913_POW2_MAX 0x8000000000000000ULL

/* ---- freestanding helpers ---------------------------------------------- */

/* Smear bits after (n-1); 0 on overflow past 2^63. */
static uint64_t
b2913_round_up(uint64_t u64N)
{
	uint64_t u64V;

	if (u64N == 0u) {
		return 1u;
	}
	if (u64N > B2913_POW2_MAX) {
		return 0u;
	}
	u64V = u64N - 1u;
	u64V |= u64V >> 1;
	u64V |= u64V >> 2;
	u64V |= u64V >> 4;
	u64V |= u64V >> 8;
	u64V |= u64V >> 16;
	u64V |= u64V >> 32;
	u64V += 1u;
	return u64V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_round_up_pow2_u - ceil n to the next power of two.
 *
 * n: value to round up
 *
 * Returns 1 for n==0; n when n is already 2^k; 0 when the next power
 * would be 2^64 (not representable). Does not call libc.
 */
uint64_t
gj_u64_round_up_pow2_u(uint64_t u64N)
{
	(void)NULL;
	return b2913_round_up(u64N);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_round_up_pow2_u(uint64_t u64N)
    __attribute__((alias("gj_u64_round_up_pow2_u")));
