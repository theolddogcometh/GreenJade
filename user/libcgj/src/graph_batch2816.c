/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2816: scalar ASCII case-fold upper (_u wave).
 *
 * Surface (unique symbols):
 *   unsigned char gj_ascii_case_upper_u(unsigned char c);
 *     - Map a..z (0x61..0x7A) to A..Z; leave all other octets unchanged.
 *       Pure scalar; no string walk, no locale.
 *   unsigned char __gj_ascii_case_upper_u  (alias)
 *   __libcgj_batch2816_marker = "libcgj-batch2816"
 *
 * Encoding/ASCII exclusive wave (2811-2820). Distinct from
 * gj_ascii_toupper_u (batch2418, same scalar idea, different symbol),
 * gj_ascii_toupper (string, batch355), and ctype toupper — unique
 * name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2816_marker[] = "libcgj-batch2816";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one unsigned octet: a..z → A..Z; else identity. */
static unsigned char
b2816_case_upper(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'a' && u8Ch <= (unsigned char)'z') {
		return (unsigned char)(u8Ch - (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_case_upper_u - fold one ASCII lowercase letter to uppercase.
 *
 * c: input octet
 *
 * Only 0x61..0x7A change; all other values are returned unchanged.
 */
unsigned char
gj_ascii_case_upper_u(unsigned char u8C)
{
	(void)NULL;
	return b2816_case_upper(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned char __gj_ascii_case_upper_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_case_upper_u")));
