/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1121: HTTP/1.1 tchar predicate (RFC 7230).
 *
 * Surface (unique symbols):
 *   int gj_http_is_token_char(int ch);
 *     — Return 1 if ch is an RFC 7230 tchar:
 *       "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /
 *       "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA
 *       (ch treated as unsigned octet; values outside 0..255 → 0).
 *   int __gj_http_is_token_char  (alias)
 *   __libcgj_batch1121_marker = "libcgj-batch1121"
 *
 * Distinct from generic isalnum / isprint — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1121_marker[] = "libcgj-batch1121";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_http_is_token_char — 1 if nCh is an HTTP tchar (RFC 7230 §3.2.6).
 */
int
gj_http_is_token_char(int nCh)
{
	unsigned char uCh;

	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	uCh = (unsigned char)nCh;

	if ((uCh >= 'A' && uCh <= 'Z') ||
	    (uCh >= 'a' && uCh <= 'z') ||
	    (uCh >= '0' && uCh <= '9')) {
		return 1;
	}

	switch (uCh) {
	case '!':
	case '#':
	case '$':
	case '%':
	case '&':
	case '\'':
	case '*':
	case '+':
	case '-':
	case '.':
	case '^':
	case '_':
	case '`':
	case '|':
	case '~':
		return 1;
	default:
		return 0;
	}
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_http_is_token_char(int nCh)
    __attribute__((alias("gj_http_is_token_char")));
