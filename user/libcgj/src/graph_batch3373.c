/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3373: standard base32 alphabet value map (_u).
 *
 * Surface (unique symbols):
 *   int gj_base32_char_value_u(unsigned char c);
 *     - Map RFC 4648 base32 alphabet char to 0..31.
 *       A-Z → 0..25, 2-7 → 26..31. Other octets (incl. '=') → -1.
 *   int __gj_base32_char_value_u  (alias)
 *   __libcgj_batch3373_marker = "libcgj-batch3373"
 *
 * CREATE-ONLY exclusive continuum wave (3371-3380): base64_char_value_u,
 * base64_is_pad_u, base32_char_value_u, hex_pair_to_u8_u, u8_to_hex_pair_u,
 * url_is_unreserved_u, url_is_gen_delim_u, url_is_sub_delim_u,
 * url_is_reserved_u, batch_id_3380. Distinct from full base32 encode/decode
 * TUs — value map only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3373_marker[] = "libcgj-batch3373";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3373_b32_val — RFC 4648 §6 alphabet value, or -1 if not in alphabet.
 * Case-sensitive uppercase only (A-Z, 2-7). Pad '=' → -1.
 */
static int
b3373_b32_val(unsigned char u8C)
{
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') {
		return (int)(u8C - (unsigned char)'A');
	}
	if (u8C >= (unsigned char)'2' && u8C <= (unsigned char)'7') {
		return (int)(u8C - (unsigned char)'2') + 26;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base32_char_value_u - map one standard base32 alphabet octet to 0..31.
 *
 * c: candidate alphabet character (uppercase RFC 4648)
 *
 * Returns 0..31 for A-Z2-7, else -1 (including pad '=' and lowercase).
 * No parent wires; does not call libc.
 */
int
gj_base32_char_value_u(unsigned char u8C)
{
	(void)NULL;
	return b3373_b32_val(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_base32_char_value_u(unsigned char u8C)
    __attribute__((alias("gj_base32_char_value_u")));
