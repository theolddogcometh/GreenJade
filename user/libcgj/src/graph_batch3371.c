/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3371: standard base64 alphabet value map (_u).
 *
 * Surface (unique symbols):
 *   int gj_base64_char_value_u(unsigned char c);
 *     - Map RFC 4648 base64 alphabet char to 0..63.
 *       A-Z → 0..25, a-z → 26..51, 0-9 → 52..61, '+' → 62, '/' → 63.
 *       Other octets (incl. '=') → -1.
 *   int __gj_base64_char_value_u  (alias)
 *   __libcgj_batch3371_marker = "libcgj-batch3371"
 *
 * CREATE-ONLY exclusive continuum wave (3371-3380): base64_char_value_u,
 * base64_is_pad_u, base32_char_value_u, hex_pair_to_u8_u, u8_to_hex_pair_u,
 * url_is_unreserved_u, url_is_gen_delim_u, url_is_sub_delim_u,
 * url_is_reserved_u, batch_id_3380. Distinct from gj_str_is_base64_char
 * (batch2079) and full base64 encode/decode TUs — value map only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3371_marker[] = "libcgj-batch3371";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3371_b64_val — RFC 4648 §4 alphabet value, or -1 if not in alphabet.
 * Padding '=' is not a value (returns -1).
 */
static int
b3371_b64_val(unsigned char u8C)
{
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') {
		return (int)(u8C - (unsigned char)'A');
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') {
		return (int)(u8C - (unsigned char)'a') + 26;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return (int)(u8C - (unsigned char)'0') + 52;
	}
	if (u8C == (unsigned char)'+') {
		return 62;
	}
	if (u8C == (unsigned char)'/') {
		return 63;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64_char_value_u - map one standard base64 alphabet octet to 0..63.
 *
 * c: candidate alphabet character
 *
 * Returns 0..63 for A-Za-z0-9+/, else -1 (including pad '='). No parent
 * wires; does not call libc.
 */
int
gj_base64_char_value_u(unsigned char u8C)
{
	(void)NULL;
	return b3371_b64_val(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_base64_char_value_u(unsigned char u8C)
    __attribute__((alias("gj_base64_char_value_u")));
