/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3017: reverse all 16 bits of a halfword
 * (exclusive rev_bits_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_rev_bits_u(uint16_t x);
 *     - Bit 0 <-> bit 15, bit 1 <-> bit 14, ... SWAR parallel reverse.
 *   uint16_t __gj_u16_rev_bits_u  (alias)
 *   __libcgj_batch3017_marker = "libcgj-batch3017"
 *
 * Exclusive continuum CREATE-ONLY wave 3011-3020. Distinct from
 * gj_bitrev16_u (batch2919), gj_bitrev8 / gj_bitrev32 / gj_bitrev64,
 * gj_u8_rev_bits_u (batch3016). Unique surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3017_marker[] = "libcgj-batch3017";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR reverse over 16 bits: 1, 2, 4, then byte swap. */
static uint16_t
b3017_rev(uint16_t u16X)
{
	uint16_t u16V;

	u16V = u16X;
	u16V = (uint16_t)(((u16V >> 1) & 0x5555u) | ((u16V & 0x5555u) << 1));
	u16V = (uint16_t)(((u16V >> 2) & 0x3333u) | ((u16V & 0x3333u) << 2));
	u16V = (uint16_t)(((u16V >> 4) & 0x0f0fu) | ((u16V & 0x0f0fu) << 4));
	u16V = (uint16_t)((u16V >> 8) | (u16V << 8));
	return u16V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_rev_bits_u - reverse the bit order of a 16-bit halfword.
 *
 * x: value whose bits are to be reversed
 *
 * Example: 0x0001 -> 0x8000; 0x00ff -> 0xff00; 0x0000 -> 0x0000.
 * Does not call libc.
 */
uint16_t
gj_u16_rev_bits_u(uint16_t u16X)
{
	(void)NULL;
	return b3017_rev(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_rev_bits_u(uint16_t u16X)
    __attribute__((alias("gj_u16_rev_bits_u")));
