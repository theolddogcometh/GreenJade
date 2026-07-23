/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4528: freestanding u8 bit reverse via LUT (_u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_bit_reverse_u(uint8_t x);
 *     - Reverse the bit order of an 8-bit value using a 16-entry nibble
 *       reverse LUT (bit 0 <-> bit 7, bit 1 <-> bit 6, ...).
 *       Example: 0x01 -> 0x80; 0xf0 -> 0x0f; 0x00 -> 0x00.
 *   uint8_t __gj_u8_bit_reverse_u  (alias)
 *   __libcgj_batch4528_marker = "libcgj-batch4528"
 *
 * Exclusive continuum CREATE-ONLY (4521-4530: table-lookup unique wave).
 * Unique gj_u8_bit_reverse_u surface only; no multi-def. Distinct from
 * gj_u8_bit_reverse (batch2131, SWAR, no _u suffix), gj_bitrev8
 * (batch560), and gj_u32_bit_reverse_u (batch3951). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4528_marker[] = "libcgj-batch4528";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Reverse the 4 bits of a nibble. Index 0..15 -> bit-reversed nibble.
 *   0 1 2 3 4 5 6 7 8 9 A B C D E F
 *   0 8 4 C 2 A 6 E 1 9 5 D 3 B 7 F
 */
static const uint8_t b4528_rev_nibble[16] = {
	0x0u, 0x8u, 0x4u, 0xcu, 0x2u, 0xau, 0x6u, 0xeu,
	0x1u, 0x9u, 0x5u, 0xdu, 0x3u, 0xbu, 0x7u, 0xfu
};

static uint8_t
b4528_bitrev(uint8_t u8X)
{
	uint8_t u8Lo;
	uint8_t u8Hi;

	u8Lo = (uint8_t)(u8X & 0x0fu);
	u8Hi = (uint8_t)((u8X >> 4) & 0x0fu);
	/* rev(hi)|rev(lo)<<4 places original bit7 into bit0, etc. */
	return (uint8_t)((b4528_rev_nibble[u8Hi]) |
	    (uint8_t)(b4528_rev_nibble[u8Lo] << 4));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_bit_reverse_u - reverse the bit order of an 8-bit byte (LUT).
 *
 * x: value whose bits are to be reversed
 *
 * Example: 0x01 -> 0x80; 0x80 -> 0x01; 0xf0 -> 0x0f; 0x00 -> 0x00.
 * No parent wires.
 */
uint8_t
gj_u8_bit_reverse_u(uint8_t u8X)
{
	(void)NULL;
	return b4528_bitrev(u8X);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_bit_reverse_u(uint8_t u8X)
    __attribute__((alias("gj_u8_bit_reverse_u")));
