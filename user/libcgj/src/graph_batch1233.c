/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1233: shell argument needs-quote predicate.
 *
 * Surface (unique symbols):
 *   int gj_shell_needs_quote(const char *s);
 *     — Return 1 if s should be quoted before embedding in a shell
 *       command: NULL, empty, or any character that gj_shell_is_metachar
 *       (batch917) would treat as special (IFS, operators, quotes,
 *       expansion). Otherwise 0. Cap scan at 4096 bytes.
 *   int __gj_shell_needs_quote  (alias)
 *   __libcgj_batch1233_marker = "libcgj-batch1233"
 *
 * Distinct from gj_shell_is_metachar (single char) and CSV/TOML quote
 * helpers — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

enum { B1233_SCAN_MAX = 4096 };

const char __libcgj_batch1233_marker[] = "libcgj-batch1233";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1233_is_meta(int nCh)
{
	switch (nCh) {
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_needs_quote — 1 if s is empty/NULL or contains shell meta.
 */
int
gj_shell_needs_quote(const char *szS)
{
	size_t i;

	if (szS == NULL || szS[0] == '\0') {
		return 1;
	}

	i = 0u;
	while (i < (size_t)B1233_SCAN_MAX && szS[i] != '\0') {
		if (b1233_is_meta((unsigned char)szS[i])) {
			return 1;
		}
		i++;
	}
	/* Unterminated within cap: treat as needs quote (conservative). */
	if (i >= (size_t)B1233_SCAN_MAX && szS[i] != '\0') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shell_needs_quote(const char *szS)
    __attribute__((alias("gj_shell_needs_quote")));
