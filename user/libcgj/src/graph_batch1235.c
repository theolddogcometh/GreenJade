/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1235: HTML ASCII space predicate.
 *
 * Surface (unique symbols):
 *   int gj_html_is_space(int ch);
 *     — Return 1 if ch is an HTML ASCII space character (HTML5
 *       "ASCII whitespace"): U+0009 TAB, U+000A LF, U+000C FF,
 *       U+000D CR, U+0020 SPACE. Else 0. ch outside 0..255 → 0.
 *   int __gj_html_is_space  (alias)
 *   __libcgj_batch1235_marker = "libcgj-batch1235"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1235_marker[] = "libcgj-batch1235";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_html_is_space — HTML5 ASCII whitespace.
 */
int
gj_html_is_space(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 0;
	}
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

/* ---- underscored alias ------------------------------------------------- */

int __gj_html_is_space(int nCh)
    __attribute__((alias("gj_html_is_space")));
