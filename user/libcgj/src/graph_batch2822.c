/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2822: ASCII whitespace single-octet predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_isspace_u(unsigned char c);
 *     - Return 1 if c is space (0x20), tab (0x09), CR (0x0D), or LF
 *       (0x0A); else 0. Does not treat VT/FF/NBSP or locale isspace
 *       classes as space.
 *   int __gj_ascii_isspace_u  (alias)
 *   __libcgj_batch2822_marker = "libcgj-batch2822"
 *
 * ASCII/path exclusive wave (2821-2830). Distinct from isspace /
 * isspace_l and gj_ascii_is_space_n (batch660 span) — unique single-
 * octet _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2822_marker[] = "libcgj-batch2822";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is space / tab / CR / LF. */
static int
b2822_isspace(unsigned char u8C)
{
	if (u8C == (unsigned char)' ') {
		return 1;
	}
	if (u8C == (unsigned char)'\t') {
		return 1;
	}
	if (u8C == (unsigned char)'\r') {
		return 1;
	}
	if (u8C == (unsigned char)'\n') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isspace_u - true when c is ASCII horizontal/vertical space.
 *
 * c: candidate octet (0..255)
 *
 * Returns 1 for SP/TAB/CR/LF only; VT/FF/NBSP and all others → 0.
 * Does not call libc isspace.
 */
int
gj_ascii_isspace_u(unsigned char u8C)
{
	(void)NULL;
	return b2822_isspace(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isspace_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_isspace_u")));
