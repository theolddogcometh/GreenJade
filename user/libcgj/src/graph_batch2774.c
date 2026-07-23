/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2774: next power of two for uint32_t
 * (exclusive continuum wave 2771-2780).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_next_pow2_u(uint32_t x);
 *     - Smallest power of two >= x. x == 0 -> 1.
 *       If x > 0x80000000 (no representable power of two >= x in
 *       uint32_t), returns 0 (overflow).
 *   uint32_t __gj_u32_next_pow2_u  (alias)
 *   __libcgj_batch2774_marker = "libcgj-batch2774"
 *
 * Distinct from gj_u32_next_pow2 (batch778) / gj_u32_is_pow2_u
 * (batch2772). Unique gj_u32_next_pow2_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2774_marker[] = "libcgj-batch2774";

/* Highest representable power of two in uint32_t. */
#define B2774_POW2_MAX 0x80000000u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round x up to next power of two. x in [1, B2774_POW2_MAX]; caller
 * guarantees domain. Already-pow2 values round to themselves via the
 * classic (x-1) smear +1 path.
 */
static uint32_t
b2774_next_pow2(uint32_t u32X)
{
	uint32_t u32V;

	u32V = u32X - 1u;
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
 * gj_u32_next_pow2_u - ceil to power of two; 0 on overflow past 2^31.
 *
 * x: input
 *   0            -> 1
 *   already pow2 -> x
 *   > 2^31       -> 0 (2^32 not representable in uint32_t)
 */
uint32_t
gj_u32_next_pow2_u(uint32_t u32X)
{
	(void)NULL;
	if (u32X == 0u) {
		return 1u;
	}
	if (u32X > B2774_POW2_MAX) {
		return 0u;
	}
	/* Already a power of two? */
	if ((u32X & (u32X - 1u)) == 0u) {
		return u32X;
	}
	return b2774_next_pow2(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_next_pow2_u(uint32_t u32X)
    __attribute__((alias("gj_u32_next_pow2_u")));
