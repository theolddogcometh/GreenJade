/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4554: ASCII hex nibble decode (_u, 0xff
 * invalid).
 *
 * Surface (unique symbols):
 *   unsigned gj_hex_decode_nibble_u(char c);
 *     - If c is a hex digit ('0'-'9', 'a'-'f', 'A'-'F'), return its
 *       nibble value 0..15. Otherwise return 0xff (invalid).
 *   unsigned __gj_hex_decode_nibble_u  (alias)
 *   __libcgj_batch4554_marker = "libcgj-batch4554"
 *
 * Exclusive continuum CREATE-ONLY (4551-4560: hex_digit_u,
 * hex_digit_upper_u, hex_encode_byte_u, hex_decode_nibble_u,
 * hex_decode_byte_u, hex_encode_len_u, hex_decode_len_u, is_hex_char_u,
 * hex_nybble_pair_u, batch_id_4560). Distinct from gj_hex_nibble
 * (batch531, int/-1), gj_hex_nibble_u (batch2796), and
 * gj_hex_nibble_val_u (batch4289, unsigned code unit) — unique
 * gj_hex_decode_nibble_u surface (char arg, 0xff invalid); no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4554_marker[] = "libcgj-batch4554";

/* Invalid nibble sentinel (not a 4-bit value). */
#define B4554_INV  0xffu

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one ASCII hex digit to 0..15; else B4554_INV. */
static unsigned
b4554_nibble(char ch)
{
	unsigned uC;

	uC = (unsigned)(unsigned char)ch;
	if (uC >= (unsigned)'0' && uC <= (unsigned)'9') {
		return uC - (unsigned)'0';
	}
	if (uC >= (unsigned)'a' && uC <= (unsigned)'f') {
		return uC - (unsigned)'a' + 10u;
	}
	if (uC >= (unsigned)'A' && uC <= (unsigned)'F') {
		return uC - (unsigned)'A' + 10u;
	}
	return B4554_INV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_decode_nibble_u - map one ASCII hex digit to a 4-bit value.
 *
 * c: candidate character
 *
 * Returns 0..15 for valid hex digits, or 0xff if c is not in
 * 0-9 / a-f / A-F. Case-insensitive for letters. Pure; no side effects.
 */
unsigned
gj_hex_decode_nibble_u(char ch)
{
	(void)NULL;
	return b4554_nibble(ch);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_hex_decode_nibble_u(char ch)
    __attribute__((alias("gj_hex_decode_nibble_u")));
