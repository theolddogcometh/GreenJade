/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2821: ASCII alphanumeric single-octet predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_isalnum_u(unsigned char c);
 *     - Return 1 if c is an ASCII letter or decimal digit
 *       ('0'..'9', 'A'..'Z', 'a'..'z'), else 0. Locale-independent;
 *       high-bit octets always fail.
 *   int __gj_ascii_isalnum_u  (alias)
 *   __libcgj_batch2821_marker = "libcgj-batch2821"
 *
 * ASCII/path exclusive wave (2821-2830). Distinct from isalnum /
 * isalnum_l (locale ctype) and gj_ascii_is_alnum_n (batch659 span) —
 * unique single-octet _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2821_marker[] = "libcgj-batch2821";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is ASCII alnum (digit or letter). */
static int
b2821_isalnum(unsigned char u8C)
{
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') {
		return 1;
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isalnum_u - true when c is an ASCII alphanumeric octet.
 *
 * c: candidate octet (0..255)
 *
 * Returns 1 for '0'..'9' / 'A'..'Z' / 'a'..'z', else 0.
 * Does not call libc isalnum.
 */
int
gj_ascii_isalnum_u(unsigned char u8C)
{
	(void)NULL;
	return b2821_isalnum(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isalnum_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_isalnum_u")));
