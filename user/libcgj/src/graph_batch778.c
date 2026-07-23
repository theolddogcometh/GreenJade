/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch778: freestanding next power of two (uint32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_next_pow2(uint32_t x);
 *     — Smallest power of two >= x. x == 0 → 1.
 *       If x > 0x80000000 (no representable power of two >= x in
 *       uint32_t), returns 0 (overflow).
 *   uint32_t __gj_u32_next_pow2  (alias)
 *   __libcgj_batch778_marker = "libcgj-batch778"
 *
 * Distinct from gj_u32_is_pow2 (batch777) / gj_u32_ilog2 (batch776).
 * Unique gj_u32_next_pow2 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch778_marker[] = "libcgj-batch778";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_next_pow2 — round x up to the next power of two.
 *
 * x: input
 *   0           → 1
 *   already pow2 → x
 *   > 0x80000000 → 0 (cannot represent next power of two in 32 bits)
 *
 * Uses smear-bits then +1; does not call libm.
 */
uint32_t
gj_u32_next_pow2(uint32_t u32X)
{
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
	u32X--;
	u32X |= u32X >> 1;
	u32X |= u32X >> 2;
	u32X |= u32X >> 4;
	u32X |= u32X >> 8;
	u32X |= u32X >> 16;
	u32X++;
	return u32X;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_next_pow2(uint32_t u32X)
    __attribute__((alias("gj_u32_next_pow2")));
