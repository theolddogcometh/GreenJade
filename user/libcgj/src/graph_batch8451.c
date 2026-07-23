/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8451: nibble → lowercase hex digit stub.
 *
 * Surface (unique symbols):
 *   int gj_hex_digit_u_8451(unsigned n);
 *     - If n is in 0..15, return the lowercase hex digit character
 *       ('0'-'9', 'a'-'f') as int. Otherwise return -1.
 *   int __gj_hex_digit_u_8451  (alias)
 *   __libcgj_batch8451_marker = "libcgj-batch8451"
 *
 * Exclusive continuum CREATE-ONLY (8451-8460: hex encode/decode stubs —
 * digit, nibble, byte_hi, byte_lo, encode_need, decode_need, is_digit,
 * upper_id, lower_id, batch_id). Distinct from gj_from_hex_nibble_u
 * (batch2797), gj_hex_char_from_nibble_u (batch2589) — unique
 * gj_hex_digit_u_8451 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8451_marker[] = "libcgj-batch8451";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map nibble 0..15 to lowercase hex digit; else -1. */
static int
b8451_digit(unsigned uNib)
{
	if (uNib < 10u) {
		return (int)('0' + uNib);
	}
	if (uNib < 16u) {
		return (int)('a' + (uNib - 10u));
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_digit_u_8451 - map a 4-bit value to a lowercase hex digit.
 *
 * n: nibble value (valid range 0..15).
 *
 * Returns '0'..'9' / 'a'..'f' as int, or -1 if n > 15. Pure; no I/O.
 * No parent wires.
 */
int
gj_hex_digit_u_8451(unsigned uNib)
{
	(void)NULL;
	return b8451_digit(uNib);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hex_digit_u_8451(unsigned uNib)
    __attribute__((alias("gj_hex_digit_u_8451")));
