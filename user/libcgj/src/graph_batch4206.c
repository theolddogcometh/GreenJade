/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4206: next power of two for uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_next_pow2_u2(uint32_t x);
 *     - Smallest power of two >= x.
 *       x == 0 → 0.
 *       If no representable power of two >= x in uint32_t (x >
 *       0x80000000), returns 0 (overflow).
 *   uint32_t __gj_u32_next_pow2_u2  (alias)
 *   __libcgj_batch4206_marker = "libcgj-batch4206"
 *
 * Exclusive continuum CREATE-ONLY (4201-4210). Distinct from
 * gj_u32_next_pow2 (batch778) / gj_u32_next_pow2_soft (batch2245) —
 * unique gj_u32_next_pow2_u2 continuum surface; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4206_marker[] = "libcgj-batch4206";

/* Highest representable power of two in uint32_t. */
#define B4206_POW2_MAX 0x80000000u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round x up to next power of two. x in [1, B4206_POW2_MAX]; caller
 * guarantees domain. Already-pow2 values round to themselves via the
 * classic (x-1) smear +1 path.
 */
static uint32_t
b4206_next_pow2(uint32_t u32X)
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
 * gj_u32_next_pow2_u2 - ceil to power of two; 0 on zero or overflow.
 *
 * x: input
 *   0            → 0
 *   already pow2 → x
 *   > 2^31       → 0 (2^32 not representable in uint32_t)
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_next_pow2_u2(uint32_t u32X)
{
	(void)NULL;
	if (u32X == 0u) {
		return 0u;
	}
	if (u32X > B4206_POW2_MAX) {
		return 0u;
	}
	/* Already a power of two? */
	if ((u32X & (u32X - 1u)) == 0u) {
		return u32X;
	}
	return b4206_next_pow2(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_next_pow2_u2(uint32_t u32X)
    __attribute__((alias("gj_u32_next_pow2_u2")));
