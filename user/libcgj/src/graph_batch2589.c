/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2589: hex char from nibble (exclusive _u).
 *
 * Surface (unique symbols):
 *   int gj_hex_char_from_nibble_u(unsigned n);
 *     - If n is in 0..15, return the lowercase hex digit character
 *       ('0'-'9', 'a'-'f') as int. Otherwise return -1.
 *   int __gj_hex_char_from_nibble_u  (alias)
 *   __libcgj_batch2589_marker = "libcgj-batch2589"
 *
 * Inverse of gj_hex_nibble_from_char_u (batch2588). Distinct from
 * encode surfaces that embed digit tables — exclusive _u surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2589_marker[] = "libcgj-batch2589";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map nibble 0..15 to lowercase hex digit; else -1. */
static int
b2589_char(unsigned uNib)
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
 * gj_hex_char_from_nibble_u — map a 4-bit value to a lowercase hex digit.
 *
 * n: nibble value (valid range 0..15).
 * Returns '0'..'9' / 'a'..'f' as int, or -1 if n > 15. Pure; no I/O.
 */
int
gj_hex_char_from_nibble_u(unsigned uNib)
{
	(void)NULL;
	return b2589_char(uNib);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hex_char_from_nibble_u(unsigned uNib)
    __attribute__((alias("gj_hex_char_from_nibble_u")));
