/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2132: freestanding uint16 full bit reverse.
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_bit_reverse(uint16_t x);
 *     - Reverse bit order of a 16-bit value (bit 0 <-> bit 15, ...).
 *       SWAR parallel reverse; pure integer, no builtins or tables.
 *   uint16_t __gj_u16_bit_reverse  (alias)
 *   __libcgj_batch2132_marker = "libcgj-batch2132"
 *
 * Post-2130 bit reverse exclusive wave (2131-2140). Unique surface;
 * no multi-def with gj_bitrev8 / gj_bit_reverse32 / gj_u8_bit_reverse.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2132_marker[] = "libcgj-batch2132";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR reverse of all 16 bits: stages 1, 2, 4, then byte swap. */
static uint16_t
b2132_bitrev(uint16_t u16X)
{
	uint16_t u16V;

	u16V = u16X;
	u16V = (uint16_t)(((u16V >> 1) & 0x5555u) | ((u16V & 0x5555u) << 1));
	u16V = (uint16_t)(((u16V >> 2) & 0x3333u) | ((u16V & 0x3333u) << 2));
	u16V = (uint16_t)(((u16V >> 4) & 0x0f0fu) | ((u16V & 0x0f0fu) << 4));
	u16V = (uint16_t)(((u16V >> 8) & 0x00ffu) | ((u16V & 0x00ffu) << 8));
	return u16V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_bit_reverse - reverse the bit order of a 16-bit word.
 *
 * x: value whose bits are to be reversed
 *
 * Example: 0x0001 -> 0x8000; 0x8000 -> 0x0001; 0xff00 -> 0x00ff.
 */
uint16_t
gj_u16_bit_reverse(uint16_t u16X)
{
	(void)NULL;
	return b2132_bitrev(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_bit_reverse(uint16_t u16X)
    __attribute__((alias("gj_u16_bit_reverse")));
