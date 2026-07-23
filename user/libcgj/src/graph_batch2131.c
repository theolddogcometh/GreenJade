/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2131: freestanding uint8 full bit reverse.
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_bit_reverse(uint8_t x);
 *     - Reverse bit order of an 8-bit value (bit 0 <-> bit 7, ...).
 *       SWAR parallel reverse; pure integer, no builtins or tables.
 *   uint8_t __gj_u8_bit_reverse  (alias)
 *   __libcgj_batch2131_marker = "libcgj-batch2131"
 *
 * Post-2130 bit reverse exclusive wave (2131-2140). Distinct from
 * gj_bitrev8 (batch560) - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2131_marker[] = "libcgj-batch2131";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR reverse of all 8 bits: stages 1, 2, then nibble swap. */
static uint8_t
b2131_bitrev(uint8_t u8X)
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
 * gj_u8_bit_reverse - reverse the bit order of an 8-bit byte.
 *
 * x: value whose bits are to be reversed
 *
 * Example: 0x01 -> 0x80; 0x80 -> 0x01; 0xf0 -> 0x0f; 0x00 -> 0x00.
 */
uint8_t
gj_u8_bit_reverse(uint8_t u8X)
{
	(void)NULL;
	return b2131_bitrev(u8X);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_bit_reverse(uint8_t u8X)
    __attribute__((alias("gj_u8_bit_reverse")));
