/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3277: ASCII nibble → lowercase hex digit.
 *
 * Surface (unique symbols):
 *   int gj_ascii_tohex_u(unsigned char n);
 *     - If n is in 0..15, return the lowercase hex digit character
 *       ('0'-'9', 'a'-'f') as int. Otherwise return -1.
 *   int __gj_ascii_tohex_u  (alias)
 *   __libcgj_batch3277_marker = "libcgj-batch3277"
 *
 * ASCII exclusive wave (3271-3280). Distinct from
 * gj_hex_char_from_nibble_u (batch2589, unsigned arg) — unique
 * ascii_tohex_u surface with unsigned char; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3277_marker[] = "libcgj-batch3277";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map nibble 0..15 to lowercase hex digit; else -1. */
static int
b3277_tohex(unsigned char u8N)
{
	if (u8N < 10u) {
		return (int)('0' + (unsigned)u8N);
	}
	if (u8N < 16u) {
		return (int)('a' + (unsigned)(u8N - 10u));
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_tohex_u - map a 4-bit value to a lowercase ASCII hex digit.
 *
 * n: nibble value (valid range 0..15).
 * Returns '0'..'9' / 'a'..'f' as int, or -1 if n > 15. Pure; no I/O.
 */
int
gj_ascii_tohex_u(unsigned char u8N)
{
	(void)NULL;
	return b3277_tohex(u8N);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_tohex_u(unsigned char u8N)
    __attribute__((alias("gj_ascii_tohex_u")));
