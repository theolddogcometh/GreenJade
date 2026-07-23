/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3562: popcount of a half-open bit range in a
 * uint32_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_bit_count_range_u(uint32_t x, unsigned lo, unsigned hi);
 *     - Number of set bits of x in half-open range [lo, hi).
 *       lo and hi are clamped to [0, 32]. After clamp, lo >= hi → 0.
 *   unsigned __gj_u32_bit_count_range_u  (alias)
 *   __libcgj_batch3562_marker = "libcgj-batch3562"
 *
 * Exclusive continuum CREATE-ONLY (3561-3570). Distinct from
 * gj_bit_count_range (batch655 uint64) — unique u32 range surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3562_marker[] = "libcgj-batch3562";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * SWAR popcount of a 32-bit word (Hacker's Delight). No multiplies; safe
 * under -ffreestanding -fno-builtin without -mpopcnt.
 */
static unsigned
b3562_pop32(uint32_t u32X)
{
	u32X = u32X - ((u32X >> 1) & 0x55555555u);
	u32X = (u32X & 0x33333333u) + ((u32X >> 2) & 0x33333333u);
	u32X = (u32X + (u32X >> 4)) & 0x0f0f0f0fu;
	u32X = u32X + (u32X >> 8);
	u32X = u32X + (u32X >> 16);
	return (unsigned)(u32X & 0x3fu);
}

/*
 * Mask with bits [0, n) set. n is in 0..32; n == 32 yields all ones.
 * Avoids undefined 1u << 32.
 */
static uint32_t
b3562_lo_mask(unsigned uN)
{
	if (uN >= 32u) {
		return ~(uint32_t)0u;
	}
	if (uN == 0u) {
		return 0u;
	}
	return ((uint32_t)1u << uN) - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bit_count_range_u - popcount of bits [lo, hi) in a uint32_t.
 *
 * x:  word to inspect (bit 0 = LSB).
 * lo: inclusive start bit index (clamped to 0..32).
 * hi: exclusive end bit index (clamped to 0..32).
 *
 * Returns the number of 1-bits in the half-open range after clamp
 * (0 when the range is empty). Does not call libc. No parent wires.
 */
unsigned
gj_u32_bit_count_range_u(uint32_t u32X, unsigned uLo, unsigned uHi)
{
	uint32_t uMask;

	(void)NULL;
	if (uLo > 32u) {
		uLo = 32u;
	}
	if (uHi > 32u) {
		uHi = 32u;
	}
	if (uLo >= uHi) {
		return 0u;
	}

	/* bits [lo, hi) = lo-mask(hi) without lo-mask(lo) */
	uMask = b3562_lo_mask(uHi) & ~b3562_lo_mask(uLo);
	return b3562_pop32(u32X & uMask);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_bit_count_range_u(uint32_t u32X, unsigned uLo, unsigned uHi)
    __attribute__((alias("gj_u32_bit_count_range_u")));
