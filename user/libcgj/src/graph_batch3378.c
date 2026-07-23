/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3378: RFC 3986 sub-delims predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_url_is_sub_delim_u(unsigned char c);
 *     - Return 1 if c is RFC 3986 §2.2 sub-delims:
 *       "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "=";
 *       else 0.
 *   int __gj_url_is_sub_delim_u  (alias)
 *   __libcgj_batch3378_marker = "libcgj-batch3378"
 *
 * CREATE-ONLY exclusive continuum wave (3371-3380): base64_char_value_u,
 * base64_is_pad_u, base32_char_value_u, hex_pair_to_u8_u, u8_to_hex_pair_u,
 * url_is_unreserved_u, url_is_gen_delim_u, url_is_sub_delim_u,
 * url_is_reserved_u, batch_id_3380. Predicate only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3378_marker[] = "libcgj-batch3378";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3378_is_sub_delim — RFC 3986 §2.2 sub-delims:
 *   "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
 */
static int
b3378_is_sub_delim(unsigned char u8C)
{
	if (u8C == (unsigned char)'!' || u8C == (unsigned char)'$' ||
	    u8C == (unsigned char)'&' || u8C == (unsigned char)'\'' ||
	    u8C == (unsigned char)'(' || u8C == (unsigned char)')' ||
	    u8C == (unsigned char)'*' || u8C == (unsigned char)'+' ||
	    u8C == (unsigned char)',' || u8C == (unsigned char)';' ||
	    u8C == (unsigned char)'=') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_url_is_sub_delim_u - 1 if c is RFC 3986 sub-delim, else 0.
 *
 * c: candidate octet
 *
 * Does not call libc. No parent wires.
 */
int
gj_url_is_sub_delim_u(unsigned char u8C)
{
	(void)NULL;
	return b3378_is_sub_delim(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_url_is_sub_delim_u(unsigned char u8C)
    __attribute__((alias("gj_url_is_sub_delim_u")));
