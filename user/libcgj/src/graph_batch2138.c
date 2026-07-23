/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2138: freestanding uint64 nibble reverse.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_nibble_reverse(uint64_t x);
 *     - Reverse the order of the sixteen 4-bit nibbles in a 64-bit word
 *       (nibble 0 <-> nibble 15, ...). Pure integer SWAR; no builtins.
 *   uint64_t __gj_u64_nibble_reverse  (alias)
 *   __libcgj_batch2138_marker = "libcgj-batch2138"
 *
 * Post-2130 bit reverse exclusive wave (2131-2140). Distinct from
 * gj_u32_nibble_reverse and bit/byte reverses - unique surface;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2138_marker[] = "libcgj-batch2138";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Reverse sixteen 4-bit lanes: adjacent-nibble swap, then bytes,
 * halfwords, and 32-bit halves.
 */
static uint64_t
b2138_nibrev(uint64_t u64X)
{
	uint64_t u64V;

	u64V = u64X;
	u64V = ((u64V & 0x0f0f0f0f0f0f0f0full) << 4)
	     | ((u64V & 0xf0f0f0f0f0f0f0f0ull) >> 4);
	u64V = ((u64V & 0x00ff00ff00ff00ffull) << 8)
	     | ((u64V & 0xff00ff00ff00ff00ull) >> 8);
	u64V = ((u64V & 0x0000ffff0000ffffull) << 16)
	     | ((u64V & 0xffff0000ffff0000ull) >> 16);
	u64V = (u64V << 32) | (u64V >> 32);
	return u64V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_nibble_reverse - reverse the sixteen nibbles of a 64-bit word.
 *
 * x: value whose nibble order is to be reversed
 *
 * Example: 0x0123456789abcdef -> 0xfedcba9876543210.
 */
uint64_t
gj_u64_nibble_reverse(uint64_t u64X)
{
	(void)NULL;
	return b2138_nibrev(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_nibble_reverse(uint64_t u64X)
    __attribute__((alias("gj_u64_nibble_reverse")));
