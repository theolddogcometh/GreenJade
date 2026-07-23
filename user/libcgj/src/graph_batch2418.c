/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2418: scalar ASCII uppercase of one unsigned
 * octet.
 *
 * Surface (unique symbols):
 *   unsigned char gj_ascii_toupper_u(unsigned char c);
 *     - Map a..z (0x61..0x7A) to A..Z; leave all other octets unchanged.
 *       Pure scalar; no string walk, no locale.
 *   unsigned char __gj_ascii_toupper_u  (alias)
 *   __libcgj_batch2418_marker = "libcgj-batch2418"
 *
 * Distinct from gj_ascii_toupper (in-place string, batch355),
 * gj_ascii_toupper_n (fixed span, batch505), and ctype toupper — unique
 * scalar u-suffix surface; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2418_marker[] = "libcgj-batch2418";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one unsigned octet: a..z -> A..Z; else identity. */
static unsigned char
b2418_to_upper(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'a' && u8Ch <= (unsigned char)'z') {
		return (unsigned char)(u8Ch - (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_toupper_u - fold one ASCII lowercase letter to uppercase.
 *
 * c: input octet
 *
 * Only 0x61..0x7A change; all other values are returned unchanged.
 */
unsigned char
gj_ascii_toupper_u(unsigned char u8C)
{
	(void)NULL;
	return b2418_to_upper(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned char __gj_ascii_toupper_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_toupper_u")));
