/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3372: base64 padding octet predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_base64_is_pad_u(unsigned char c);
 *     - Return 1 if c is the base64 padding character '=' (0x3D); else 0.
 *   int __gj_base64_is_pad_u  (alias)
 *   __libcgj_batch3372_marker = "libcgj-batch3372"
 *
 * CREATE-ONLY exclusive continuum wave (3371-3380): base64_char_value_u,
 * base64_is_pad_u, base32_char_value_u, hex_pair_to_u8_u, u8_to_hex_pair_u,
 * url_is_unreserved_u, url_is_gen_delim_u, url_is_sub_delim_u,
 * url_is_reserved_u, batch_id_3380. Distinct from gj_str_is_base64_char
 * (batch2079) — pad predicate only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3372_marker[] = "libcgj-batch3372";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is the RFC 4648 padding character '='. */
static int
b3372_is_pad(unsigned char u8C)
{
	if (u8C == (unsigned char)'=') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64_is_pad_u - 1 if c is base64 padding '=', else 0.
 *
 * c: candidate octet
 *
 * Does not call libc. No parent wires.
 */
int
gj_base64_is_pad_u(unsigned char u8C)
{
	(void)NULL;
	return b3372_is_pad(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_base64_is_pad_u(unsigned char u8C)
    __attribute__((alias("gj_base64_is_pad_u")));
