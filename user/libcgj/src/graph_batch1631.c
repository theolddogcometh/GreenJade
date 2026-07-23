/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1631: single-word bitset popcount (u64).
 *
 * Surface (unique symbols):
 *   unsigned gj_bitset64_popcount(uint64_t x);
 *     — Number of 1-bits in a 64-bit bitset word (0..64). SWAR path;
 *       no compiler builtins / libgcc popcount dependency.
 *   unsigned __gj_bitset64_popcount  (alias)
 *   __libcgj_batch1631_marker = "libcgj-batch1631"
 *
 * Distinct from gj_popcount64 (batch139), gj_bitset_popcount (batch245),
 * and gj_bitset_pop_all (batch492) — unique scalar bitset64 surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained.
 */

#include <stdint.h>

const char __libcgj_batch1631_marker[] = "libcgj-batch1631";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR Hamming weight of one uint64_t (Hacker's Delight style). */
static unsigned
b1631_pop64(uint64_t u64X)
{
	u64X = u64X - ((u64X >> 1) & 0x5555555555555555ull);
	u64X = (u64X & 0x3333333333333333ull) +
	    ((u64X >> 2) & 0x3333333333333333ull);
	u64X = (u64X + (u64X >> 4)) & 0x0f0f0f0f0f0f0f0full;
	u64X = u64X + (u64X >> 8);
	u64X = u64X + (u64X >> 16);
	u64X = u64X + (u64X >> 32);
	return (unsigned)(u64X & 0x7full);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_popcount — population count of a 64-bit bitset word.
 *
 * x: dense 64-bit bitset (bit 0 = LSB).
 *
 * Returns the number of set bits in [0, 64). x == 0 → 0.
 */
unsigned
gj_bitset64_popcount(uint64_t u64X)
{
	return b1631_pop64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_bitset64_popcount(uint64_t u64X)
    __attribute__((alias("gj_bitset64_popcount")));
