/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3873: C-like identifier continue octet predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_ident_cont_u(int ch);
 *     - Return 1 if ch is an ASCII identifier-continue character:
 *       [A-Za-z0-9_]. ch outside 0..255 → 0. ASCII only; no locale.
 *   int __gj_str_is_ident_cont_u  (alias)
 *   __libcgj_batch3873_marker = "libcgj-batch3873"
 *
 * Exclusive continuum CREATE-ONLY (3871-3880). Distinct from
 * gj_css_is_ident_char (batch1239) and gj_str_is_ident_start_u
 * (batch3872) — unique gj_str_is_ident_cont_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3873_marker[] = "libcgj-batch3873";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3873_is_cont(int nCh)
{
	unsigned char u8C;

	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	u8C = (unsigned char)nCh;
	if ((u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ||
	    (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') ||
	    (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') ||
	    u8C == (unsigned char)'_') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_ident_cont_u - 1 if ch may continue a C-like identifier.
 *
 * Accepts A-Z / a-z / '0'-'9' / '_'. Other octets → 0.
 * Does not call libc. No parent wires.
 */
int
gj_str_is_ident_cont_u(int nCh)
{
	(void)NULL;
	return b3873_is_cont(nCh);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_ident_cont_u(int nCh)
    __attribute__((alias("gj_str_is_ident_cont_u")));
