/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1238: CSS identifier start predicate (ASCII).
 *
 * Surface (unique symbols):
 *   int gj_css_is_ident_start(int ch);
 *     — Return 1 if ch may start a CSS <ident-token> in the ASCII /
 *       non-ASCII subset used here: [A-Za-z] | '_' | non-ASCII
 *       (ch >= 0x80 and ch <= 0xff). Digits and '-' alone are not
 *       start chars (CSS allows leading '--' as two chars; caller
 *       handles that). ch outside 0..255 → 0.
 *   int __gj_css_is_ident_start  (alias)
 *   __libcgj_batch1238_marker = "libcgj-batch1238"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1238_marker[] = "libcgj-batch1238";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_css_is_ident_start — CSS ident start (ASCII + high-bit).
 */
int
gj_css_is_ident_start(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	if ((nCh >= 'A' && nCh <= 'Z') || (nCh >= 'a' && nCh <= 'z')) {
		return 1;
	}
	if (nCh == '_') {
		return 1;
	}
	if (nCh >= 0x80) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_css_is_ident_start(int nCh)
    __attribute__((alias("gj_css_is_ident_start")));
