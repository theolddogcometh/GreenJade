/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4529: freestanding u16 bit reverse via LUT (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_bit_reverse_u(uint16_t x);
 *     - Reverse the bit order of a 16-bit value using a 16-entry nibble
 *       reverse LUT applied to each of the four nibbles, then
 *       reassembly in reverse nibble order.
 *       Example: 0x0001 -> 0x8000; 0xf000 -> 0x000f.
 *   uint16_t __gj_u16_bit_reverse_u  (alias)
 *   __libcgj_batch4529_marker = "libcgj-batch4529"
 *
 * Exclusive continuum CREATE-ONLY (4521-4530: table-lookup unique wave).
 * Unique gj_u16_bit_reverse_u surface only; no multi-def. Distinct from
 * gj_u16_bit_reverse (batch2132, no _u suffix), gj_u8_bit_reverse_u
 * (batch4528), and gj_u32_bit_reverse_u (batch3951). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4529_marker[] = "libcgj-batch4529";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Reverse the 4 bits of a nibble. Index 0..15 -> bit-reversed nibble.
 */
static const uint8_t b4529_rev_nibble[16] = {
	0x0u, 0x8u, 0x4u, 0xcu, 0x2u, 0xau, 0x6u, 0xeu,
	0x1u, 0x9u, 0x5u, 0xdu, 0x3u, 0xbu, 0x7u, 0xfu
};

static uint16_t
b4529_bitrev(uint16_t u16X)
{
	uint8_t u8N0;
	uint8_t u8N1;
	uint8_t u8N2;
	uint8_t u8N3;
	uint16_t u16R;

	u8N0 = (uint8_t)(u16X & 0x0fu);
	u8N1 = (uint8_t)((u16X >> 4) & 0x0fu);
	u8N2 = (uint8_t)((u16X >> 8) & 0x0fu);
	u8N3 = (uint8_t)((u16X >> 12) & 0x0fu);
	/*
	 * Reversed layout: rev(n3) at bits 0..3, rev(n2) at 4..7,
	 * rev(n1) at 8..11, rev(n0) at 12..15.
	 */
	u16R  = (uint16_t)b4529_rev_nibble[u8N3];
	u16R |= (uint16_t)b4529_rev_nibble[u8N2] << 4;
	u16R |= (uint16_t)b4529_rev_nibble[u8N1] << 8;
	u16R |= (uint16_t)b4529_rev_nibble[u8N0] << 12;
	return u16R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_bit_reverse_u - reverse the bit order of a 16-bit word (LUT).
 *
 * x: value whose bits are to be reversed
 *
 * Example: 0x0001 -> 0x8000; 0x8000 -> 0x0001; 0xf000 -> 0x000f.
 * No parent wires.
 */
uint16_t
gj_u16_bit_reverse_u(uint16_t u16X)
{
	(void)NULL;
	return b4529_bitrev(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_bit_reverse_u(uint16_t u16X)
    __attribute__((alias("gj_u16_bit_reverse_u")));
