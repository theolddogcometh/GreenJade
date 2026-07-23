/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3956: freestanding uint64 nibble reverse (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_nibble_reverse_u(uint64_t x);
 *     - Reverse the order of the sixteen 4-bit nibbles in a 64-bit word
 *       (nibble 0 <-> nibble 15, ...). Pure integer SWAR; no builtins.
 *   uint64_t __gj_u64_nibble_reverse_u  (alias)
 *   __libcgj_batch3956_marker = "libcgj-batch3956"
 *
 * CREATE-ONLY exclusive continuum wave (3951-3960). Unique
 * gj_u64_nibble_reverse_u surface only; no multi-def. Distinct from
 * gj_u64_nibble_reverse (batch2138) and bit/byte reverse surfaces.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3956_marker[] = "libcgj-batch3956";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Reverse sixteen 4-bit lanes: adjacent-nibble swap, then bytes,
 * halfwords, and 32-bit halves.
 */
static uint64_t
b3956_nibrev(uint64_t u64X)
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
 * gj_u64_nibble_reverse_u - reverse the sixteen nibbles of a 64-bit word.
 *
 * x: value whose nibble order is to be reversed
 *
 * Example: 0x0123456789abcdef -> 0xfedcba9876543210.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_nibble_reverse_u(uint64_t u64X)
{
	(void)NULL;
	return b3956_nibrev(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_nibble_reverse_u(uint64_t u64X)
    __attribute__((alias("gj_u64_nibble_reverse_u")));
