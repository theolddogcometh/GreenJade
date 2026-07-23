/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch655: popcount of a half-open bit range in a
 * uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_bit_count_range(uint64_t x, unsigned lo, unsigned hi);
 *     — Number of set bits of x in half-open range [lo, hi).
 *       lo and hi are clamped to [0, 64]. After clamp, lo >= hi → 0.
 *   unsigned __gj_bit_count_range  (alias)
 *   __libcgj_batch655_marker = "libcgj-batch655"
 *
 * Does NOT redefine gj_popcount64 (batch139), gj_bitset_popcount
 * (batch245), or other bitset surfaces — single-word range only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch655_marker[] = "libcgj-batch655";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * SWAR popcount of a 64-bit word (Hacker's Delight). No multiplies; safe
 * under -ffreestanding -fno-builtin without -mpopcnt.
 */
static unsigned
b655_pop64(uint64_t x)
{
	x = x - ((x >> 1) & 0x5555555555555555ull);
	x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0full;
	x = x + (x >> 8);
	x = x + (x >> 16);
	x = x + (x >> 32);
	return (unsigned)(x & 0x7full);
}

/*
 * Mask with bits [0, n) set. n is in 0..64; n == 64 yields all ones.
 * Avoids undefined 1ull << 64.
 */
static uint64_t
b655_lo_mask(unsigned n)
{
	if (n >= 64u) {
		return ~(uint64_t)0;
	}
	if (n == 0u) {
		return (uint64_t)0;
	}
	return ((uint64_t)1 << n) - (uint64_t)1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_count_range — popcount of bits [lo, hi) in a uint64_t.
 *
 * x:  word to inspect (bit 0 = LSB).
 * lo: inclusive start bit index (clamped to 0..64).
 * hi: exclusive end bit index (clamped to 0..64).
 *
 * Returns the number of 1-bits in the half-open range after clamp
 * (0 when the range is empty).
 */
unsigned
gj_bit_count_range(uint64_t x, unsigned lo, unsigned hi)
{
	uint64_t uMask;

	if (lo > 64u) {
		lo = 64u;
	}
	if (hi > 64u) {
		hi = 64u;
	}
	if (lo >= hi) {
		return 0u;
	}

	/* bits [lo, hi) = lo-mask(hi) without lo-mask(lo) */
	uMask = b655_lo_mask(hi) & ~b655_lo_mask(lo);
	return b655_pop64(x & uMask);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_bit_count_range(uint64_t x, unsigned lo, unsigned hi)
    __attribute__((alias("gj_bit_count_range")));
