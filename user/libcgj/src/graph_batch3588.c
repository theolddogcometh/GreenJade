/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3588: ASCII whitespace predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ws_is_space_u(unsigned char c);
 *     - Return 1 if c is ASCII whitespace as defined by C isspace for
 *       the "C" locale: space, form-feed, newline, carriage-return,
 *       horizontal tab, vertical tab (0x09..0x0D and 0x20). Else 0.
 *       Pure scalar; no locale tables.
 *   int __gj_ws_is_space_u  (alias)
 *   __libcgj_batch3588_marker = "libcgj-batch3588"
 *
 * Exclusive continuum CREATE-ONLY (3581-3590). Distinct from ctype
 * isspace / WebSocket gj_ws_* frames — unique ws_is_space_u surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3588_marker[] = "libcgj-batch3588";

/* ---- freestanding helpers ---------------------------------------------- */

/* C-locale ASCII whitespace: HT LF VT FF CR SP. */
static int
b3588_is_space(unsigned char u8C)
{
	if (u8C == (unsigned char)' ') {
		return 1;
	}
	if (u8C >= 0x09u && u8C <= 0x0Du) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ws_is_space_u - 1 if c is ASCII whitespace, else 0.
 *
 * c: input octet
 */
int
gj_ws_is_space_u(unsigned char u8C)
{
	(void)NULL;
	return b3588_is_space(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ws_is_space_u(unsigned char u8C)
    __attribute__((alias("gj_ws_is_space_u")));
