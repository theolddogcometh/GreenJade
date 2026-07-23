/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1635: next set bit strictly after index i.
 *
 * Surface (unique symbols):
 *   int gj_bitset64_next_bit_after(uint64_t x, int i);
 *     — Index of the next set bit with index > i, or -1 if none.
 *       Bits are numbered 0..63 (LSB = 0). i may be negative (scan
 *       from bit 0) or >= 63 (always -1).
 *   int __gj_bitset64_next_bit_after  (alias)
 *   __libcgj_batch1635_marker = "libcgj-batch1635"
 *
 * Distinct from gj_bitset_next (batch209, multi-word, inclusive start)
 * — unique single-word exclusive-after surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained.
 */

#include <stdint.h>

const char __libcgj_batch1635_marker[] = "libcgj-batch1635";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count trailing zeros; u64X must be nonzero. */
static unsigned
b1635_ctz64(uint64_t u64X)
{
	unsigned uCtz;
	uint64_t u64V;

	u64V = u64X;
	uCtz = 0u;

	if ((u64V & 0xffffffffull) == 0ull) {
		uCtz += 32u;
		u64V >>= 32;
	}
	if ((u64V & 0xffffull) == 0ull) {
		uCtz += 16u;
		u64V >>= 16;
	}
	if ((u64V & 0xffull) == 0ull) {
		uCtz += 8u;
		u64V >>= 8;
	}
	if ((u64V & 0xfull) == 0ull) {
		uCtz += 4u;
		u64V >>= 4;
	}
	if ((u64V & 0x3ull) == 0ull) {
		uCtz += 2u;
		u64V >>= 2;
	}
	if ((u64V & 0x1ull) == 0ull) {
		uCtz += 1u;
	}

	return uCtz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_next_bit_after — next set bit index strictly greater than i.
 *
 * x: dense 64-bit bitset (bit 0 = LSB).
 * i: exclusive lower bound on bit index.
 *
 * Returns j in (i, 63] with bit j set and no smaller such j, or -1.
 */
int
gj_bitset64_next_bit_after(uint64_t u64X, int iBit)
{
	uint64_t u64Masked;
	unsigned uShift;

	if (u64X == 0ull) {
		return -1;
	}
	if (iBit >= 63) {
		return -1;
	}

	if (iBit < 0) {
		u64Masked = u64X;
	} else {
		/* Clear bits 0..iBit inclusive; keep bits (iBit+1)..63. */
		uShift = (unsigned)iBit + 1u; /* 1..63 */
		u64Masked = u64X & (~0ull << uShift);
	}

	if (u64Masked == 0ull) {
		return -1;
	}

	return (int)b1635_ctz64(u64Masked);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset64_next_bit_after(uint64_t u64X, int iBit)
    __attribute__((alias("gj_bitset64_next_bit_after")));
