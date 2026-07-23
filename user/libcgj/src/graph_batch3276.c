/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3276: ASCII blank single-octet predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_isblank_u(unsigned char c);
 *     - Return 1 if c is space (0x20) or horizontal tab (0x09); else 0.
 *   int __gj_ascii_isblank_u  (alias)
 *   __libcgj_batch3276_marker = "libcgj-batch3276"
 *
 * ASCII exclusive wave (3271-3280). Distinct from isblank / isblank_l
 * and gj_ascii_isspace_u (batch2822, also CR/LF) — unique _u surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3276_marker[] = "libcgj-batch3276";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is space or HT. */
static int
b3276_isblank(unsigned char u8C)
{
	if (u8C == (unsigned char)' ') {
		return 1;
	}
	if (u8C == (unsigned char)'\t') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isblank_u - true when c is ASCII blank (SP or HT).
 *
 * c: candidate octet (0..255)
 *
 * Returns 1 for space/tab only; CR/LF/VT/FF and all others → 0.
 * Does not call libc isblank.
 */
int
gj_ascii_isblank_u(unsigned char u8C)
{
	(void)NULL;
	return b3276_isblank(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isblank_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_isblank_u")));
