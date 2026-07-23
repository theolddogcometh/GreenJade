/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1239: CSS identifier continue predicate (ASCII).
 *
 * Surface (unique symbols):
 *   int gj_css_is_ident_char(int ch);
 *     — Return 1 if ch may continue a CSS <ident-token>: start chars
 *       (letter / '_' / non-ASCII 0x80..0xff) plus digit and '-'.
 *       Else 0. ch outside 0..255 → 0.
 *   int __gj_css_is_ident_char  (alias)
 *   __libcgj_batch1239_marker = "libcgj-batch1239"
 *
 * Distinct from gj_css_is_ident_start (batch1238) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1239_marker[] = "libcgj-batch1239";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_css_is_ident_char — CSS ident continue (ASCII + high-bit).
 */
int
gj_css_is_ident_char(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	if ((nCh >= 'A' && nCh <= 'Z') || (nCh >= 'a' && nCh <= 'z')) {
		return 1;
	}
	if (nCh >= '0' && nCh <= '9') {
		return 1;
	}
	if (nCh == '_' || nCh == '-') {
		return 1;
	}
	if (nCh >= 0x80) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_css_is_ident_char(int nCh)
    __attribute__((alias("gj_css_is_ident_char")));
