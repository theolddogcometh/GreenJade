/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2819: ASCII alphabetic predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ascii_isalpha_u(unsigned char c);
 *     - Return 1 if c is an ASCII letter A..Z or a..z; else 0.
 *   int __gj_ascii_isalpha_u  (alias)
 *   __libcgj_batch2819_marker = "libcgj-batch2819"
 *
 * Encoding/ASCII exclusive wave (2811-2820). Distinct from isalpha
 * (ctype.c) and gj_ascii_is_alpha_n (span, batch658) — unique _u
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2819_marker[] = "libcgj-batch2819";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is in A..Z or a..z. */
static int
b2819_isalpha(unsigned char u8C)
{
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
 * gj_ascii_isalpha_u - 1 iff c is an ASCII alphabetic letter.
 *
 * Edge cases:
 *   'A'..'Z' / 'a'..'z' → 1
 *   '0'..'9' / ' ' / any other → 0
 */
int
gj_ascii_isalpha_u(unsigned char u8C)
{
	(void)NULL;
	return b2819_isalpha(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isalpha_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_isalpha_u")));
