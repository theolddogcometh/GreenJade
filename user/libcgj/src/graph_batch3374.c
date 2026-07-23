/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3374: two hex digits → byte (_u).
 *
 * Surface (unique symbols):
 *   int gj_hex_pair_to_u8_u(unsigned char hi, unsigned char lo);
 *     - Parse two hex ASCII nibbles into one octet value 0..255.
 *       Accepts 0-9 / a-f / A-F for each nibble.
 *       Returns the combined byte on success, or -1 if either nibble
 *       is not a valid hex digit.
 *   int __gj_hex_pair_to_u8_u  (alias)
 *   __libcgj_batch3374_marker = "libcgj-batch3374"
 *
 * CREATE-ONLY exclusive continuum wave (3371-3380): base64_char_value_u,
 * base64_is_pad_u, base32_char_value_u, hex_pair_to_u8_u, u8_to_hex_pair_u,
 * url_is_unreserved_u, url_is_gen_delim_u, url_is_sub_delim_u,
 * url_is_reserved_u, batch_id_3380. Distinct from MAC/UUID hex helpers —
 * pair-only surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3374_marker[] = "libcgj-batch3374";

/* ---- freestanding helpers ---------------------------------------------- */

/* Nibble 0..15, or -1 if not a hex digit. */
static int
b3374_nibble(unsigned char u8C)
{
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return (int)(u8C - (unsigned char)'0');
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'f') {
		return (int)(u8C - (unsigned char)'a') + 10;
	}
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'F') {
		return (int)(u8C - (unsigned char)'A') + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_pair_to_u8_u - combine two hex ASCII digits into one byte.
 *
 * hi: high nibble character
 * lo: low nibble character
 *
 * Returns 0..255 on success, -1 if either digit is invalid. Does not
 * call libc. No parent wires.
 */
int
gj_hex_pair_to_u8_u(unsigned char u8Hi, unsigned char u8Lo)
{
	int nHi;
	int nLo;

	(void)NULL;
	nHi = b3374_nibble(u8Hi);
	nLo = b3374_nibble(u8Lo);
	if (nHi < 0 || nLo < 0) {
		return -1;
	}
	return (nHi << 4) | nLo;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hex_pair_to_u8_u(unsigned char u8Hi, unsigned char u8Lo)
    __attribute__((alias("gj_hex_pair_to_u8_u")));
