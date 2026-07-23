/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2245: next power of two for uint32_t
 * (post-2240 math exclusive wave 2241-2250).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_next_pow2_soft(uint32_t x);
 *     - Smallest power of two >= x. x == 0 -> 1.
 *       If x > 0x80000000 (no representable power of two >= x in
 *       uint32_t), returns 0 (overflow).
 *   uint32_t __gj_u32_next_pow2_soft  (alias)
 *   __libcgj_batch2245_marker = "libcgj-batch2245"
 *
 * Renamed from the planned gj_u32_next_pow2 because that symbol is
 * owned by batch778; _soft suffix keeps the surface unique (no
 * multi-def). Also distinct from gj_u32_is_pow2 (batch777) /
 * gj_u32_ilog2 (batch776).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2245_marker[] = "libcgj-batch2245";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round x up to next power of two. x in [1, 0x80000000]; caller
 * guarantees domain. Already-pow2 values round to themselves via the
 * classic (x-1) smear +1 path.
 */
static uint32_t
b2245_next_pow2(uint32_t u32X)
{
	u32X--;
	u32X |= u32X >> 1;
	u32X |= u32X >> 2;
	u32X |= u32X >> 4;
	u32X |= u32X >> 8;
	u32X |= u32X >> 16;
	u32X++;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_next_pow2_soft - round x up to the next power of two.
 *
 * x: input
 *   0            -> 1
 *   already pow2 -> x
 *   > 0x80000000 -> 0 (cannot represent next power of two in 32 bits)
 *
 * Uses smear-bits then +1; does not call libm.
 */
uint32_t
gj_u32_next_pow2_soft(uint32_t u32X)
{
	(void)NULL;
	if (u32X == 0u) {
		return 1u;
	}
	if (u32X > 0x80000000u) {
		return 0u;
	}
	/* Already a power of two? */
	if ((u32X & (u32X - 1u)) == 0u) {
		return u32X;
	}
	return b2245_next_pow2(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_next_pow2_soft(uint32_t u32X)
    __attribute__((alias("gj_u32_next_pow2_soft")));
