/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1123: HTTP method token validation.
 *
 * Surface (unique symbols):
 *   int gj_http_method_ok(const char *method);
 *     — Return 1 if method is a non-empty sequence of tchars
 *       (RFC 7230/7231 method = token). Length 1..64. NULL / empty → 0.
 *   int __gj_http_method_ok  (alias)
 *   __libcgj_batch1123_marker = "libcgj-batch1123"
 *
 * Distinct from gj_http_is_token_char (batch1121) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1123_marker[] = "libcgj-batch1123";

/* ---- helpers ------------------------------------------------------------ */

static int
b1123_is_tchar(unsigned char uCh)
{
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_http_method_ok — 1 if szMethod is a valid HTTP method token.
 */
int
gj_http_method_ok(const char *szMethod)
{
	size_t cb;
	size_t i;

	if (szMethod == NULL) {
		return 0;
	}

	cb = 0u;
	while (szMethod[cb] != '\0') {
		cb++;
		if (cb > 64u) {
			return 0;
		}
	}
	if (cb == 0u || cb > 64u) {
		return 0;
	}

	for (i = 0u; i < cb; i++) {
		if (!b1123_is_tchar((unsigned char)szMethod[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_http_method_ok(const char *szMethod)
    __attribute__((alias("gj_http_method_ok")));
