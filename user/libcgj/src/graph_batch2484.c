/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2484: freestanding y/n boolean token parse.
 *
 * Surface (unique symbols):
 *   int gj_parse_bool_yn(const char *s);
 *     — Full-string case-insensitive ASCII match against:
 *         y / yes → 1
 *         n / no  → 0
 *       Any other token (including NULL / empty) → -1.
 *   int __gj_parse_bool_yn  (alias)
 *   __libcgj_batch2484_marker = "libcgj-batch2484"
 *
 * Distinct from gj_parse_bool (batch448), gj_parse_bool_ci (batch1078),
 * and gj_parse_bool_strict (batch2141). Post-2480 parse exclusive wave
 * (2481-2490).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2484_marker[] = "libcgj-batch2484";

/* ---- freestanding helpers ---------------------------------------------- */

static char
b2484_tolower_ascii(char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return (char)(ch - 'A' + 'a');
	}
	return ch;
}

/*
 * b2484_eq_ci — case-insensitive full-string ASCII compare to literal.
 * Returns 1 on match, 0 otherwise.
 */
static int
b2484_eq_ci(const char *sz, const char *szLit)
{
	size_t i;

	if (sz == NULL || szLit == NULL) {
		return 0;
	}
	i = 0u;
	for (;;) {
		char chA = b2484_tolower_ascii(sz[i]);
		char chB = b2484_tolower_ascii(szLit[i]);

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
 * gj_parse_bool_yn — y/n (and yes/no) token → 1 / 0 / -1.
 *
 * Accepted truthy (case-insensitive ASCII): "y", "yes"
 * Accepted falsy  (case-insensitive ASCII): "n", "no"
 *
 * Edge cases:
 *   s == NULL → -1
 *   "" / "true" / "1" / "on" / "y " → -1
 *   "Y" / "Yes" / "N" / "NO" → accepted
 */
int
gj_parse_bool_yn(const char *sz)
{
	(void)NULL;

	if (sz == NULL) {
		return -1;
	}

	if (b2484_eq_ci(sz, "y") || b2484_eq_ci(sz, "yes")) {
		return 1;
	}
	if (b2484_eq_ci(sz, "n") || b2484_eq_ci(sz, "no")) {
		return 0;
	}
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_bool_yn(const char *sz)
    __attribute__((alias("gj_parse_bool_yn")));
