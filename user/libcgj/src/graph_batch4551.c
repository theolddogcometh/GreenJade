/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4551: nibble → lowercase hex digit (_u).
 *
 * Surface (unique symbols):
 *   int gj_hex_digit_u(unsigned v);
 *     - Map (v & 15) to a lowercase hex ASCII digit ('0'-'9', 'a'-'f').
 *       Always returns a valid digit (low nibble only; no invalid path).
 *   int __gj_hex_digit_u  (alias)
 *   __libcgj_batch4551_marker = "libcgj-batch4551"
 *
 * Exclusive continuum CREATE-ONLY (4551-4560: hex_digit_u,
 * hex_digit_upper_u, hex_encode_byte_u, hex_decode_nibble_u,
 * hex_decode_byte_u, hex_encode_len_u, hex_decode_len_u, is_hex_char_u,
 * hex_nybble_pair_u, batch_id_4560). Distinct from
 * gj_hex_char_from_nibble_u (batch2589, -1 on invalid) and
 * gj_hex_nibble_u (batch2796, decode) — unique gj_hex_digit_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4551_marker[] = "libcgj-batch4551";

/* ---- freestanding helpers ---------------------------------------------- */

/* Low nibble → lowercase hex ASCII as int. */
static int
b4551_digit(unsigned uV)
{
	unsigned uNib;

	uNib = uV & 15u;
	if (uNib < 10u) {
		return (int)('0' + (int)uNib);
	}
	return (int)('a' + (int)(uNib - 10u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_digit_u - map low nibble of v to a lowercase hex digit.
 *
 * v: value whose low 4 bits select the digit (higher bits ignored)
 *
 * Returns '0'..'9' or 'a'..'f' as int. Pure; no side effects.
 * Self-contained; no parent wires.
 */
int
gj_hex_digit_u(unsigned uV)
{
	(void)NULL;
	return b4551_digit(uV);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hex_digit_u(unsigned uV)
    __attribute__((alias("gj_hex_digit_u")));
