/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2912: uint32_t round up to power-of-two
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_round_up_pow2_u(uint32_t n);
 *     - Smallest power of two >= n.
 *       n == 0 -> 1.
 *       If n is already a power of two, return n.
 *       If result would exceed 2^31 (n > 0x80000000), return 0
 *       (overflow: 2^32 not representable in uint32_t).
 *   uint32_t __gj_u32_round_up_pow2_u  (alias)
 *   __libcgj_batch2912_marker = "libcgj-batch2912"
 *
 * Exclusive continuum wave 2911-2920. Distinct from gj_next_pow2_u64
 * (batch374). Unique gj_u32_round_up_pow2_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2912_marker[] = "libcgj-batch2912";

/* Highest representable power of two in uint32_t. */
#define B2912_POW2_MAX 0x80000000u

/* ---- freestanding helpers ---------------------------------------------- */

/* Smear bits after (n-1); 0 on overflow past 2^31. */
static uint32_t
b2912_round_up(uint32_t u32N)
{
	uint32_t u32V;

	if (u32N == 0u) {
		return 1u;
	}
	if (u32N > B2912_POW2_MAX) {
		return 0u;
	}
	u32V = u32N - 1u;
	u32V |= u32V >> 1;
	u32V |= u32V >> 2;
	u32V |= u32V >> 4;
	u32V |= u32V >> 8;
	u32V |= u32V >> 16;
	u32V += 1u;
	return u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_round_up_pow2_u - ceil n to the next power of two.
 *
 * n: value to round up
 *
 * Returns 1 for n==0; n when n is already 2^k; 0 when the next power
 * would be 2^32 (not representable). Does not call libc.
 */
uint32_t
gj_u32_round_up_pow2_u(uint32_t u32N)
{
	(void)NULL;
	return b2912_round_up(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_round_up_pow2_u(uint32_t u32N)
    __attribute__((alias("gj_u32_round_up_pow2_u")));
