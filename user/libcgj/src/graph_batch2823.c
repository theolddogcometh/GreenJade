/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2823: ASCII printable single-octet predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_isprint_u(unsigned char c);
 *     - Return 1 if c is in the closed range 0x20..0x7E (ASCII space
 *       through '~'); else 0. Control octets, DEL (0x7F), and high-bit
 *       octets fail.
 *   int __gj_ascii_isprint_u  (alias)
 *   __libcgj_batch2823_marker = "libcgj-batch2823"
 *
 * ASCII/path exclusive wave (2821-2830). Distinct from isprint /
 * isprint_l and gj_ascii_is_print_n (batch661 span) — unique single-
 * octet _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2823_marker[] = "libcgj-batch2823";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is ASCII printable 0x20..0x7E. */
static int
b2823_isprint(unsigned char u8C)
{
	if (u8C >= 0x20u && u8C <= 0x7Eu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isprint_u - true when c is an ASCII printable octet.
 *
 * c: candidate octet (0..255)
 *
 * Returns 1 for 0x20..0x7E inclusive, else 0.
 * Does not call libc isprint.
 */
int
gj_ascii_isprint_u(unsigned char u8C)
{
	(void)NULL;
	return b2823_isprint(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isprint_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_isprint_u")));
