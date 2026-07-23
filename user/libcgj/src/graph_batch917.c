/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch917: shell metacharacter predicate.
 *
 * Surface (unique symbols):
 *   int gj_shell_is_metachar(int c);
 *     — Return 1 if c is a common POSIX shell metacharacter that ends
 *       or special-cases an unquoted token, else 0.
 *   int __gj_shell_is_metachar  (alias)
 *   __libcgj_batch917_marker = "libcgj-batch917"
 *
 * Metachar set (IFS + operators + expansion / quote / escape):
 *   space tab newline | & ; ( ) < > * ? [ ] { } ' " ` $ \ # !
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch917_marker[] = "libcgj-batch917";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_is_metachar — true for shell-significant characters.
 *
 * Intended for simple token-boundary scans in unquoted text. Does not
 * implement quoting state; callers track quote context separately.
 */
int
gj_shell_is_metachar(int c)
{
	switch (c) {
	case ' ':
	case '\t':
	case '\n':
	case '|':
	case '&':
	case ';':
	case '(':
	case ')':
	case '<':
	case '>':
	case '*':
	case '?':
	case '[':
	case ']':
	case '{':
	case '}':
	case '\'':
	case '"':
	case '`':
	case '$':
	case '\\':
	case '#':
	case '!':
		return 1;
	default:
		return 0;
	}
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shell_is_metachar(int c)
    __attribute__((alias("gj_shell_is_metachar")));
