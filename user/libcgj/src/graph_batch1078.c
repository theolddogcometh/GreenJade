/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1078: freestanding boolean token parse.
 *
 * NOTE: gj_parse_bool / __gj_parse_bool already live in graph_batch448.c
 * with identical true/false/1/0/yes/no semantics. This TU does NOT
 * redefine them — avoid multi-def.
 *
 * Surface (unique symbols):
 *   int gj_parse_bool_ci(const char *s);
 *     — Full-string case-insensitive ASCII match against:
 *         true / 1 / yes  → 1
 *         false / 0 / no  → 0
 *       Any other token (including NULL / empty) → -1.
 *   int __gj_parse_bool_ci  (alias)
 *   __libcgj_batch1078_marker = "libcgj-batch1078"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1078_marker[] = "libcgj-batch1078";

/* ---- helpers ----------------------------------------------------------- */

static char
b1078_tolower_ascii(char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return (char)(ch - 'A' + 'a');
	}
	return ch;
}

/*
 * b1078_eq_ci — case-insensitive full-string ASCII compare to literal.
 * Returns 1 on match, 0 otherwise.
 */
static int
b1078_eq_ci(const char *sz, const char *szLit)
{
	size_t i;

	if (sz == NULL || szLit == NULL) {
		return 0;
	}
	i = 0u;
	for (;;) {
		char chA = b1078_tolower_ascii(sz[i]);
		char chB = b1078_tolower_ascii(szLit[i]);

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
 * gj_parse_bool_ci — true/false/1/0/yes/no → 1 / 0 / -1.
 *
 * Unique alias surface for batch1078; gj_parse_bool is batch448.
 *
 * Accepted truthy (case-insensitive ASCII): "true", "1", "yes"
 * Accepted falsy  (case-insensitive ASCII): "false", "0", "no"
 */
int
gj_parse_bool_ci(const char *sz)
{
	if (sz == NULL) {
		return -1;
	}

	if (b1078_eq_ci(sz, "true") || b1078_eq_ci(sz, "1") ||
	    b1078_eq_ci(sz, "yes")) {
		return 1;
	}
	if (b1078_eq_ci(sz, "false") || b1078_eq_ci(sz, "0") ||
	    b1078_eq_ci(sz, "no")) {
		return 0;
	}
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_bool_ci(const char *sz)
    __attribute__((alias("gj_parse_bool_ci")));
