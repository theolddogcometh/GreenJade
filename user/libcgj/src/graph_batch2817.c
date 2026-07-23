/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2817: ASCII hex-digit predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ascii_isxdigit_u(unsigned char c);
 *     - Return 1 if c is an ASCII hexadecimal digit in
 *       '0'..'9', 'a'..'f', or 'A'..'F'; else 0.
 *   int __gj_ascii_isxdigit_u  (alias)
 *   __libcgj_batch2817_marker = "libcgj-batch2817"
 *
 * Encoding/ASCII exclusive wave (2811-2820). Distinct from
 * isxdigit (ctype.c), gj_is_hex_digit (batch685), and locale
 * isxdigit_l — unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2817_marker[] = "libcgj-batch2817";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is in [0-9a-fA-F]. */
static int
b2817_isxdigit(unsigned char u8C)
{
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'f') {
		return 1;
	}
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'F') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isxdigit_u - 1 iff c is an ASCII hexadecimal digit.
 *
 * Edge cases:
 *   '0'..'9' / 'a'..'f' / 'A'..'F' → 1
 *   'g' / 'G' / ' ' / any other → 0
 */
int
gj_ascii_isxdigit_u(unsigned char u8C)
{
	(void)NULL;
	return b2817_isxdigit(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isxdigit_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_isxdigit_u")));
