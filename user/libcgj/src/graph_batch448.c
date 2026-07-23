/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch448: freestanding boolean token parse.
 *
 * Surface (unique symbols):
 *   int gj_parse_bool(const char *s);
 *     — Full-string case-insensitive ASCII match against:
 *         true / 1 / yes  → 1
 *         false / 0 / no  → 0
 *       Any other token (including NULL / empty) → -1.
 *   int __gj_parse_bool  (alias)
 *   __libcgj_batch448_marker = "libcgj-batch448"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch448_marker[] = "libcgj-batch448";

/* ---- helpers ----------------------------------------------------------- */

static char
b448_tolower_ascii(char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return (char)(ch - 'A' + 'a');
	}
	return ch;
}

/*
 * b448_eq_ci — case-insensitive full-string ASCII compare to literal.
 * Returns 1 on match, 0 otherwise.
 */
static int
b448_eq_ci(const char *sz, const char *szLit)
{
	size_t i;

	if (sz == NULL || szLit == NULL) {
		return 0;
	}
	i = 0u;
	for (;;) {
		char chA = b448_tolower_ascii(sz[i]);
		char chB = b448_tolower_ascii(szLit[i]);

		if (chA != chB) {
			return 0;
		}
		if (chA == '\0') {
			return 1;
		}
		i++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_bool — true/false token → 1 / 0 / -1.
 *
 * Accepted truthy (case-insensitive ASCII): "true", "1", "yes"
 * Accepted falsy  (case-insensitive ASCII): "false", "0", "no"
 *
 * Edge cases:
 *   s == NULL → -1
 *   "" / "maybe" / "2" / "y" / "true " → -1
 *   "TRUE" / "Yes" / "FALSE" / "No" → accepted
 */
int
gj_parse_bool(const char *s)
{
	if (s == NULL) {
		return -1;
	}

	if (b448_eq_ci(s, "true") || b448_eq_ci(s, "1") ||
	    b448_eq_ci(s, "yes")) {
		return 1;
	}
	if (b448_eq_ci(s, "false") || b448_eq_ci(s, "0") ||
	    b448_eq_ci(s, "no")) {
		return 0;
	}
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_bool(const char *s)
    __attribute__((alias("gj_parse_bool")));
