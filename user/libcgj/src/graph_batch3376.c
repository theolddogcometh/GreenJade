/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3376: RFC 3986 unreserved octet predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_url_is_unreserved_u(unsigned char c);
 *     - Return 1 if c is RFC 3986 §2.3 unreserved:
 *       ALPHA / DIGIT / "-" / "." / "_" / "~"; else 0.
 *   int __gj_url_is_unreserved_u  (alias)
 *   __libcgj_batch3376_marker = "libcgj-batch3376"
 *
 * CREATE-ONLY exclusive continuum wave (3371-3380): base64_char_value_u,
 * base64_is_pad_u, base32_char_value_u, hex_pair_to_u8_u, u8_to_hex_pair_u,
 * url_is_unreserved_u, url_is_gen_delim_u, url_is_sub_delim_u,
 * url_is_reserved_u, batch_id_3380. Distinct from gj_str_url_needs_encode_p
 * (batch2071) and pct_encode TUs — predicate only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3376_marker[] = "libcgj-batch3376";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3376_is_unreserved — RFC 3986 §2.3 unreserved set:
 *   ALPHA / DIGIT / "-" / "." / "_" / "~"
 */
static int
b3376_is_unreserved(unsigned char u8C)
{
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') {
		return 1;
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	if (u8C == (unsigned char)'-' || u8C == (unsigned char)'.' ||
	    u8C == (unsigned char)'_' || u8C == (unsigned char)'~') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_url_is_unreserved_u - 1 if c is RFC 3986 unreserved, else 0.
 *
 * c: candidate octet
 *
 * Does not call libc. No parent wires.
 */
int
gj_url_is_unreserved_u(unsigned char u8C)
{
	(void)NULL;
	return b3376_is_unreserved(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_url_is_unreserved_u(unsigned char u8C)
    __attribute__((alias("gj_url_is_unreserved_u")));
