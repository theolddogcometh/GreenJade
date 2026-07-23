/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3274: ASCII punctuation single-octet predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_ispunct_u(unsigned char c);
 *     - Return 1 if c is an ASCII printable non-alnum non-space octet
 *       (0x21..0x2F, 0x3A..0x40, 0x5B..0x60, 0x7B..0x7E); else 0.
 *   int __gj_ascii_ispunct_u  (alias)
 *   __libcgj_batch3274_marker = "libcgj-batch3274"
 *
 * ASCII exclusive wave (3271-3280). Distinct from ispunct / ispunct_l
 * and gj_ascii_isprint_u (batch2823) — unique single-octet _u surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3274_marker[] = "libcgj-batch3274";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is ASCII punctuation (print && !alnum && !space). */
static int
b3274_ispunct(unsigned char u8C)
{
	if (u8C >= 0x21u && u8C <= 0x2Fu) {
		return 1;
	}
	if (u8C >= 0x3Au && u8C <= 0x40u) {
		return 1;
	}
	if (u8C >= 0x5Bu && u8C <= 0x60u) {
		return 1;
	}
	if (u8C >= 0x7Bu && u8C <= 0x7Eu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_ispunct_u - true when c is an ASCII punctuation octet.
 *
 * c: candidate octet (0..255)
 *
 * Returns 1 for the four ASCII punct ranges; alnum/space/control → 0.
 * Does not call libc ispunct.
 */
int
gj_ascii_ispunct_u(unsigned char u8C)
{
	(void)NULL;
	return b3274_ispunct(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_ispunct_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_ispunct_u")));
