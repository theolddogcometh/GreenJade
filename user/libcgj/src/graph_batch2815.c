/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2815: scalar ASCII case-fold lower (_u wave).
 *
 * Surface (unique symbols):
 *   unsigned char gj_ascii_case_lower_u(unsigned char c);
 *     - Map A..Z (0x41..0x5A) to a..z; leave all other octets unchanged.
 *       Pure scalar; no string walk, no locale.
 *   unsigned char __gj_ascii_case_lower_u  (alias)
 *   __libcgj_batch2815_marker = "libcgj-batch2815"
 *
 * Encoding/ASCII exclusive wave (2811-2820). Distinct from
 * gj_ascii_tolower_u (batch2417, same scalar idea, different symbol),
 * gj_ascii_tolower (string, batch355), and ctype tolower — unique
 * name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2815_marker[] = "libcgj-batch2815";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one unsigned octet: A..Z → a..z; else identity. */
static unsigned char
b2815_case_lower(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_case_lower_u - fold one ASCII uppercase letter to lowercase.
 *
 * c: input octet
 *
 * Only 0x41..0x5A change; all other values are returned unchanged.
 */
unsigned char
gj_ascii_case_lower_u(unsigned char u8C)
{
	(void)NULL;
	return b2815_case_lower(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned char __gj_ascii_case_lower_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_case_lower_u")));
