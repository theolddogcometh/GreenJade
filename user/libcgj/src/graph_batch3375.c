/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3375: byte → two lowercase hex digits (_u).
 *
 * Surface (unique symbols):
 *   int gj_u8_to_hex_pair_u(uint8_t v, char *out);
 *     - Write two lowercase hex ASCII digits for v into out[0] and out[1]
 *       (no trailing NUL). Returns 0 on success, -1 if out is NULL.
 *   int __gj_u8_to_hex_pair_u  (alias)
 *   __libcgj_batch3375_marker = "libcgj-batch3375"
 *
 * CREATE-ONLY exclusive continuum wave (3371-3380): base64_char_value_u,
 * base64_is_pad_u, base32_char_value_u, hex_pair_to_u8_u, u8_to_hex_pair_u,
 * url_is_unreserved_u, url_is_gen_delim_u, url_is_sub_delim_u,
 * url_is_reserved_u, batch_id_3380. Distinct from full hex_encode TUs —
 * pair-only surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3375_marker[] = "libcgj-batch3375";

/* ---- freestanding helpers ---------------------------------------------- */

/* Nibble 0..15 → lowercase hex ASCII '0'-'9' / 'a'-'f'. */
static char
b3375_digit(unsigned uNib)
{
	if (uNib < 10u) {
		return (char)('0' + (char)uNib);
	}
	return (char)('a' + (char)(uNib - 10u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_to_hex_pair_u - encode one byte as two lowercase hex digits.
 *
 * v:   octet to encode (0..255)
 * out: destination of at least 2 chars; not NUL-terminated by this call
 *
 * Returns 0 on success, -1 if out is NULL. Does not call libc. No parent
 * wires.
 */
int
gj_u8_to_hex_pair_u(uint8_t u8V, char *pOut)
{
	(void)NULL;
	if (pOut == NULL) {
		return -1;
	}
	pOut[0] = b3375_digit(((unsigned)u8V >> 4) & 0xfu);
	pOut[1] = b3375_digit((unsigned)u8V & 0xfu);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u8_to_hex_pair_u(uint8_t u8V, char *pOut)
    __attribute__((alias("gj_u8_to_hex_pair_u")));
