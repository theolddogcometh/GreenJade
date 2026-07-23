/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1231: basic regex metacharacter predicate.
 *
 * Surface (unique symbols):
 *   int gj_regex_is_meta(int ch);
 *     — Return 1 if ch is a common basic/extended regex metacharacter:
 *       . ^ $ * + ? ( ) [ ] { } | \
 *       else 0. ch outside 0..255 → 0.
 *   int __gj_regex_is_meta  (alias)
 *   __libcgj_batch1231_marker = "libcgj-batch1231"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1231_marker[] = "libcgj-batch1231";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_regex_is_meta — true for common regex metacharacters.
 */
int
gj_regex_is_meta(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	switch (nCh) {
	case '.':
	case '^':
	case '$':
	case '*':
	case '+':
	case '?':
	case '(':
	case ')':
	case '[':
	case ']':
	case '{':
	case '}':
	case '|':
	case '\\':
		return 1;
	default:
		return 0;
	}
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_regex_is_meta(int nCh)
    __attribute__((alias("gj_regex_is_meta")));
