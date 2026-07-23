/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3379: RFC 3986 reserved octet predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_url_is_reserved_u(unsigned char c);
 *     - Return 1 if c is RFC 3986 §2.2 reserved (gen-delims / sub-delims):
 *       gen-delims: ":" / "/" / "?" / "#" / "[" / "]" / "@"
 *       sub-delims: "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" /
 *                   "," / ";" / "="
 *       else 0. Self-contained (no calls into sibling batch TUs).
 *   int __gj_url_is_reserved_u  (alias)
 *   __libcgj_batch3379_marker = "libcgj-batch3379"
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

const char __libcgj_batch3379_marker[] = "libcgj-batch3379";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3379_is_reserved — RFC 3986 §2.2 reserved = gen-delims / sub-delims.
 * Inlined here (no parent wires to batch3377/3378).
 */
static int
b3379_is_reserved(unsigned char u8C)
{
	/* gen-delims */
	if (u8C == (unsigned char)':' || u8C == (unsigned char)'/' ||
	    u8C == (unsigned char)'?' || u8C == (unsigned char)'#' ||
	    u8C == (unsigned char)'[' || u8C == (unsigned char)']' ||
	    u8C == (unsigned char)'@') {
		return 1;
	}
	/* sub-delims */
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
 * gj_url_is_reserved_u - 1 if c is RFC 3986 reserved, else 0.
 *
 * c: candidate octet
 *
 * Self-contained gen-delims ∪ sub-delims check. Does not call sibling
 * batch symbols or libc. No parent wires.
 */
int
gj_url_is_reserved_u(unsigned char u8C)
{
	(void)NULL;
	return b3379_is_reserved(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_url_is_reserved_u(unsigned char u8C)
    __attribute__((alias("gj_url_is_reserved_u")));
