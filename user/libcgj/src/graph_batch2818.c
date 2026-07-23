/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2818: ASCII decimal-digit predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ascii_isdigit_u(unsigned char c);
 *     - Return 1 if c is an ASCII decimal digit '0'..'9'; else 0.
 *   int __gj_ascii_isdigit_u  (alias)
 *   __libcgj_batch2818_marker = "libcgj-batch2818"
 *
 * Encoding/ASCII exclusive wave (2811-2820). Distinct from isdigit
 * (ctype.c), gj_ascii_is_digit_n (span, batch657), and
 * gj_is_digit_str (string, batch449) — unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2818_marker[] = "libcgj-batch2818";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is in '0'..'9'. */
static int
b2818_isdigit(unsigned char u8C)
{
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isdigit_u - 1 iff c is an ASCII decimal digit.
 *
 * Edge cases:
 *   '0'..'9' → 1
 *   'a' / 'A' / ' ' / any other → 0
 */
int
gj_ascii_isdigit_u(unsigned char u8C)
{
	(void)NULL;
	return b2818_isdigit(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isdigit_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_isdigit_u")));
