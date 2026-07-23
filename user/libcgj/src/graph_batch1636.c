/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1636: previous set bit strictly before index i.
 *
 * Surface (unique symbols):
 *   int gj_bitset64_prev_bit_before(uint64_t x, int i);
 *     — Index of the previous set bit with index < i, or -1 if none.
 *       Bits are numbered 0..63 (LSB = 0). i <= 0 → -1; i > 64 treats
 *       all bits as candidates (same as i == 64).
 *   int __gj_bitset64_prev_bit_before  (alias)
 *   __libcgj_batch1636_marker = "libcgj-batch1636"
 *
 * Distinct from gj_bitset64_next_bit_after (batch1635) and multi-word
 * scanners — unique single-word exclusive-before surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained.
 */

#include <stdint.h>

const char __libcgj_batch1636_marker[] = "libcgj-batch1636";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count leading zeros; u64X == 0 → 64. */
static unsigned
b1636_clz64(uint64_t u64X)
{
	unsigned uClz;
	uint64_t u64V;

	if (u64X == 0ull) {
		return 64u;
	}

	u64V = u64X;
	uClz = 0u;

	if ((u64V & 0xffffffff00000000ull) == 0ull) {
		uClz += 32u;
		u64V <<= 32;
	}
	if ((u64V & 0xffff000000000000ull) == 0ull) {
		uClz += 16u;
		u64V <<= 16;
	}
	if ((u64V & 0xff00000000000000ull) == 0ull) {
		uClz += 8u;
		u64V <<= 8;
	}
	if ((u64V & 0xf000000000000000ull) == 0ull) {
		uClz += 4u;
		u64V <<= 4;
	}
	if ((u64V & 0xc000000000000000ull) == 0ull) {
		uClz += 2u;
		u64V <<= 2;
	}
	if ((u64V & 0x8000000000000000ull) == 0ull) {
		uClz += 1u;
	}

	return uClz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_prev_bit_before — previous set bit index strictly less than i.
 *
 * x: dense 64-bit bitset (bit 0 = LSB).
 * i: exclusive upper bound on bit index.
 *
 * Returns j in [0, i) with bit j set and no larger such j, or -1.
 */
int
gj_bitset64_prev_bit_before(uint64_t u64X, int iBit)
{
	uint64_t u64Masked;
	unsigned uClz;

	if (u64X == 0ull) {
		return -1;
	}
	if (iBit <= 0) {
		return -1;
	}

	if (iBit >= 64) {
		u64Masked = u64X;
	} else {
		/* Keep bits 0..(iBit-1); shift amount is 1..63. */
		u64Masked = u64X & ((1ull << (unsigned)iBit) - 1ull);
	}

	if (u64Masked == 0ull) {
		return -1;
	}

	/* MSB index = 63 - clz. */
	uClz = b1636_clz64(u64Masked);
	return (int)(63u - uClz);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset64_prev_bit_before(uint64_t u64X, int iBit)
    __attribute__((alias("gj_bitset64_prev_bit_before")));
