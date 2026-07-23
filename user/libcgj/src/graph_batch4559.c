/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4559: byte → packed lowercase hex nybble pair
 * (_u, u16).
 *
 * Surface (unique symbols):
 *   uint16_t gj_hex_nybble_pair_u(uint8_t b);
 *     - Return a u16 with two lowercase ASCII hex digits in the low
 *       bytes: bits[7:0] = high-nibble digit, bits[15:8] = low-nibble
 *       digit (LE memory order yields out[0], out[1] as encode_byte).
 *   uint16_t __gj_hex_nybble_pair_u  (alias)
 *   __libcgj_batch4559_marker = "libcgj-batch4559"
 *
 * Exclusive continuum CREATE-ONLY (4551-4560: hex_digit_u,
 * hex_digit_upper_u, hex_encode_byte_u, hex_decode_nibble_u,
 * hex_decode_byte_u, hex_encode_len_u, hex_decode_len_u, is_hex_char_u,
 * hex_nybble_pair_u, batch_id_4560). Distinct from
 * gj_hex_encode_byte_u (batch4553, out buffer) and
 * gj_u8_to_hex_pair_u (batch3375) — unique packed-u16 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4559_marker[] = "libcgj-batch4559";

/* ---- freestanding helpers ---------------------------------------------- */

/* Low nibble → lowercase hex ASCII. */
static unsigned
b4559_digit(unsigned uNib)
{
	uNib &= 15u;
	if (uNib < 10u) {
		return (unsigned)'0' + uNib;
	}
	return (unsigned)'a' + (uNib - 10u);
}

/* Pack two hex digits into low 16 bits of a u16. */
static uint16_t
b4559_pair(uint8_t u8B)
{
	unsigned uHi;
	unsigned uLo;

	uHi = b4559_digit(((unsigned)u8B >> 4) & 0xfu);
	uLo = b4559_digit((unsigned)u8B & 0xfu);
	return (uint16_t)(uHi | (uLo << 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_nybble_pair_u - pack two lowercase hex digits for b into a u16.
 *
 * b: octet to encode (0..255)
 *
 * Returns u16: low byte = high-nibble digit, high byte = low-nibble
 * digit ('0'-'9'/'a'-'f'). Pure; no side effects. No parent wires.
 */
uint16_t
gj_hex_nybble_pair_u(uint8_t u8B)
{
	(void)NULL;
	return b4559_pair(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_hex_nybble_pair_u(uint8_t u8B)
    __attribute__((alias("gj_hex_nybble_pair_u")));
