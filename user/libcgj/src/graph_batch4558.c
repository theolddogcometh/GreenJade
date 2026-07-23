/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4558: hex character predicate (_u, unsigned).
 *
 * Surface (unique symbols):
 *   int gj_is_hex_char_u(unsigned c);
 *     - Return 1 if c is a hex digit code unit in [0-9a-fA-F], else 0.
 *   int __gj_is_hex_char_u  (alias)
 *   __libcgj_batch4558_marker = "libcgj-batch4558"
 *
 * Exclusive continuum CREATE-ONLY (4551-4560: hex_digit_u,
 * hex_digit_upper_u, hex_encode_byte_u, hex_decode_nibble_u,
 * hex_decode_byte_u, hex_encode_len_u, hex_decode_len_u, is_hex_char_u,
 * hex_nybble_pair_u, batch_id_4560). Distinct from gj_is_hex_digit
 * (batch685, int c), isxdigit (ctype), and gj_is_hex_str (batch450) —
 * unique gj_is_hex_char_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4558_marker[] = "libcgj-batch4558";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff c is in [0-9a-fA-F]. */
static int
b4558_is_hex(unsigned uC)
{
	if (uC >= (unsigned)'0' && uC <= (unsigned)'9') {
		return 1;
	}
	if (uC >= (unsigned)'a' && uC <= (unsigned)'f') {
		return 1;
	}
	if (uC >= (unsigned)'A' && uC <= (unsigned)'F') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_hex_char_u - 1 iff c is an ASCII hex digit code unit.
 *
 * c: candidate code unit (typically a promoted octet)
 *
 * Returns 1 for 0-9 / a-f / A-F, else 0. Pure; no side effects.
 * Self-contained; no parent wires.
 */
int
gj_is_hex_char_u(unsigned uC)
{
	(void)NULL;
	return b4558_is_hex(uC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_hex_char_u(unsigned uC)
    __attribute__((alias("gj_is_hex_char_u")));
