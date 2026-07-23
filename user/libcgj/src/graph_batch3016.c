/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3016: reverse all 8 bits of a byte
 * (exclusive rev_bits_u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_rev_bits_u(uint8_t x);
 *     - Bit 0 <-> bit 7, bit 1 <-> bit 6, ... SWAR parallel reverse.
 *   uint8_t __gj_u8_rev_bits_u  (alias)
 *   __libcgj_batch3016_marker = "libcgj-batch3016"
 *
 * Exclusive continuum CREATE-ONLY wave 3011-3020. Distinct from
 * gj_bitrev8 (batch560), gj_bitrev32 / gj_bitrev64. Unique surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3016_marker[] = "libcgj-batch3016";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR reverse over 8 bits: adjacent, pairs, then nibbles. */
static uint8_t
b3016_rev(uint8_t u8X)
{
	uint8_t u8V;

	u8V = u8X;
	u8V = (uint8_t)(((u8V >> 1) & 0x55u) | ((u8V & 0x55u) << 1));
	u8V = (uint8_t)(((u8V >> 2) & 0x33u) | ((u8V & 0x33u) << 2));
	u8V = (uint8_t)(((u8V >> 4) & 0x0fu) | ((u8V & 0x0fu) << 4));
	return u8V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_rev_bits_u - reverse the bit order of an 8-bit byte.
 *
 * x: value whose bits are to be reversed
 *
 * Example: 0x01 -> 0x80; 0x80 -> 0x01; 0xf0 -> 0x0f; 0x00 -> 0x00.
 * Does not call libc.
 */
uint8_t
gj_u8_rev_bits_u(uint8_t u8X)
{
	(void)NULL;
	return b3016_rev(u8X);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_rev_bits_u(uint8_t u8X)
    __attribute__((alias("gj_u8_rev_bits_u")));
