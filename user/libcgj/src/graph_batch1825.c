/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1825: HTML whitespace skip (bounded).
 *
 * Surface (unique symbols):
 *   size_t gj_html_skip_ws_n(const char *s, size_t n);
 *     — Walk up to n bytes of s, counting consecutive HTML5 ASCII
 *       whitespace (TAB/LF/FF/CR/SPACE). Returns the number of
 *       whitespace bytes consumed (0..n). Stops at first non-ws or
 *       after n bytes. NULL s → 0. Does not require a trailing NUL
 *       within n (length-bounded).
 *   size_t __gj_html_skip_ws_n  (alias)
 *   __libcgj_batch1825_marker = "libcgj-batch1825"
 *
 * Companion to gj_html_is_ws (batch1824). XML/HTML tag exclusive wave
 * (1821–1829). Clean-room freestanding pure C. Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1825_marker[] = "libcgj-batch1825";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1825_is_ws(int nCh)
{
	switch (nCh) {
	case '\t':
	case '\n':
	case '\f':
	case '\r':
	case ' ':
		return 1;
	default:
		return 0;
	}
}

static size_t
b1825_skip_ws_n(const char *sz, size_t cbN)
{
	size_t i;

	if (sz == NULL || cbN == 0) {
		return 0;
	}
	i = 0;
	while (i < cbN && b1825_is_ws((unsigned char)sz[i])) {
		i++;
	}
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_html_skip_ws_n — count leading HTML whitespace within n bytes.
 *
 * sz:  buffer (may be unterminated within n)
 * cbN: maximum bytes to inspect
 *
 * Returns how many leading HTML5 ASCII whitespace bytes were found.
 * Callers may advance a cursor by the return value.
 */
size_t
gj_html_skip_ws_n(const char *sz, size_t cbN)
{
	return b1825_skip_ws_n(sz, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_html_skip_ws_n(const char *sz, size_t cbN)
    __attribute__((alias("gj_html_skip_ws_n")));
