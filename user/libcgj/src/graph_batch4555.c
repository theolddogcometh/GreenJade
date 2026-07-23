/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4555: two hex digits → one byte (_u, 1 ok).
 *
 * Surface (unique symbols):
 *   int gj_hex_decode_byte_u(char hi, char lo, uint8_t *out);
 *     - Parse hi/lo as hex ASCII nibbles; on success store the octet in
 *       *out and return 1. Return 0 if either nibble is invalid or out
 *       is NULL.
 *   int __gj_hex_decode_byte_u  (alias)
 *   __libcgj_batch4555_marker = "libcgj-batch4555"
 *
 * Exclusive continuum CREATE-ONLY (4551-4560: hex_digit_u,
 * hex_digit_upper_u, hex_encode_byte_u, hex_decode_nibble_u,
 * hex_decode_byte_u, hex_encode_len_u, hex_decode_len_u, is_hex_char_u,
 * hex_nybble_pair_u, batch_id_4560). Distinct from
 * gj_hex_pair_to_u8_u (batch3374, returns byte or -1) — unique
 * gj_hex_decode_byte_u surface (1/0 + out pointer); no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4555_marker[] = "libcgj-batch4555";

/* Invalid nibble sentinel. */
#define B4555_INV  0xffu

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one ASCII hex digit to 0..15; else B4555_INV. */
static unsigned
b4555_nibble(char ch)
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
	return B4555_INV;
}

/* Decode hi/lo into *pOut; 1 ok, 0 fail. */
static int
b4555_decode_byte(char chHi, char chLo, uint8_t *pOut)
{
	unsigned uHi;
	unsigned uLo;

	if (pOut == NULL) {
		return 0;
	}
	uHi = b4555_nibble(chHi);
	uLo = b4555_nibble(chLo);
	if (uHi == B4555_INV || uLo == B4555_INV) {
		return 0;
	}
	*pOut = (uint8_t)((uHi << 4) | uLo);
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_decode_byte_u - combine two hex ASCII digits into one byte.
 *
 * hi:  high nibble character
 * lo:  low nibble character
 * out: destination octet (NULL → fail)
 *
 * Returns 1 on success (writes *out), 0 if either digit is invalid or
 * out is NULL. Does not call libc. No parent wires.
 */
int
gj_hex_decode_byte_u(char chHi, char chLo, uint8_t *pOut)
{
	(void)NULL;
	return b4555_decode_byte(chHi, chLo, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hex_decode_byte_u(char chHi, char chLo, uint8_t *pOut)
    __attribute__((alias("gj_hex_decode_byte_u")));
