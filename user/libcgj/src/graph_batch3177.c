/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3177: popcount of a uint64_t bitset word.
 *
 * Surface (unique symbols):
 *   unsigned gj_bitset64_pop_u(uint64_t x);
 *     - Return the number of set bits in x (Hamming weight 0..64).
 *       SWAR parallel bit sum; no compiler builtins, no parent wires.
 *   unsigned __gj_bitset64_pop_u  (alias)
 *   __libcgj_batch3177_marker = "libcgj-batch3177"
 *
 * Milestone 3180 exclusive continuum CREATE-ONLY (3171-3180). Distinct
 * from gj_popcount64 (batch139), gj_bitset_popcount (batch245), and
 * gj_bitset_pop_all (batch492) — unique gj_bitset64_pop_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3177_marker[] = "libcgj-batch3177";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * SWAR popcount (Hacker's Delight style). Safe under -fno-builtin;
 * no multiplies.
 */
static unsigned
b3177_pop(uint64_t u64X)
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
 * gj_bitset64_pop_u - count set bits in a 64-bit bitset word.
 *
 * x: 64-bit bitset word
 *
 * Returns Hamming weight in 0..64. Integer-only SWAR; no floating point.
 * No parent wires.
 */
unsigned
gj_bitset64_pop_u(uint64_t u64X)
{
	(void)NULL;
	return b3177_pop(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_bitset64_pop_u(uint64_t u64X)
    __attribute__((alias("gj_bitset64_pop_u")));
