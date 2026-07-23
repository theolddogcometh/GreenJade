/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3563: popcount of a half-open bit range in a
 * uint64_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_bit_count_range_u(uint64_t x, unsigned lo, unsigned hi);
 *     - Number of set bits of x in half-open range [lo, hi).
 *       lo and hi are clamped to [0, 64]. After clamp, lo >= hi → 0.
 *   unsigned __gj_u64_bit_count_range_u  (alias)
 *   __libcgj_batch3563_marker = "libcgj-batch3563"
 *
 * Exclusive continuum CREATE-ONLY (3561-3570). Companion to
 * gj_u32_bit_count_range_u (batch3562). Distinct from
 * gj_bit_count_range (batch655) — unique u64 _u surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3563_marker[] = "libcgj-batch3563";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * SWAR popcount of a 64-bit word (Hacker's Delight). No multiplies; safe
 * under -ffreestanding -fno-builtin without -mpopcnt.
 */
static unsigned
b3563_pop64(uint64_t u64X)
{
	u64X = u64X - ((u64X >> 1) & 0x5555555555555555ull);
	u64X = (u64X & 0x3333333333333333ull) + ((u64X >> 2) & 0x3333333333333333ull);
	u64X = (u64X + (u64X >> 4)) & 0x0f0f0f0f0f0f0f0full;
	u64X = u64X + (u64X >> 8);
	u64X = u64X + (u64X >> 16);
	u64X = u64X + (u64X >> 32);
	return (unsigned)(u64X & 0x7full);
}

/*
 * Mask with bits [0, n) set. n is in 0..64; n == 64 yields all ones.
 * Avoids undefined 1ull << 64.
 */
static uint64_t
b3563_lo_mask(unsigned uN)
{
	if (uN >= 64u) {
		return ~(uint64_t)0;
	}
	if (uN == 0u) {
		return (uint64_t)0;
	}
	return ((uint64_t)1 << uN) - (uint64_t)1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_bit_count_range_u - popcount of bits [lo, hi) in a uint64_t.
 *
 * x:  word to inspect (bit 0 = LSB).
 * lo: inclusive start bit index (clamped to 0..64).
 * hi: exclusive end bit index (clamped to 0..64).
 *
 * Returns the number of 1-bits in the half-open range after clamp
 * (0 when the range is empty). Does not call libc. No parent wires.
 */
unsigned
gj_u64_bit_count_range_u(uint64_t u64X, unsigned uLo, unsigned uHi)
{
	uint64_t uMask;

	(void)NULL;
	if (uLo > 64u) {
		uLo = 64u;
	}
	if (uHi > 64u) {
		uHi = 64u;
	}
	if (uLo >= uHi) {
		return 0u;
	}

	/* bits [lo, hi) = lo-mask(hi) without lo-mask(lo) */
	uMask = b3563_lo_mask(uHi) & ~b3563_lo_mask(uLo);
	return b3563_pop64(u64X & uMask);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_bit_count_range_u(uint64_t u64X, unsigned uLo, unsigned uHi)
    __attribute__((alias("gj_u64_bit_count_range_u")));
