/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch507: case-insensitive NUL-terminated string
 * prefix predicate (starts-with, ASCII).
 *
 * Surface (unique symbols):
 *   int gj_starts_with_ci(const char *s, const char *pfx);
 *     — Return 1 if s begins with every octet of pfx under ASCII
 *       case-insensitive compare (including when pfx is empty), else 0.
 *       NULL s or NULL pfx returns 0.
 *   __gj_starts_with_ci  (alias)
 *   __libcgj_batch507_marker = "libcgj-batch507"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. Distinct from
 * gj_starts_with in graph_batch356.c (exact-byte prefix).
 */

#include <stddef.h>

const char __libcgj_batch507_marker[] = "libcgj-batch507";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A–Z → a–z; all other bytes unchanged. */
static char
b507_tolower_ascii(char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return (char)(ch - 'A' + 'a');
	}
	return ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_starts_with_ci — true if s has pfx as a leading substring (ASCII CI).
 *
 * s:   haystack C string (NULL → 0)
 * pfx: required prefix (NULL → 0; empty string matches any non-NULL s)
 *
 * Comparison folds ASCII letters only (A–Z / a–z); other octets compare
 * exactly. Stops at the first mismatch or when pfx is exhausted; s need
 * not be fully scanned. No locale.
 */
int
gj_starts_with_ci(const char *s, const char *pfx)
{
	size_t i;

	if (s == NULL || pfx == NULL) {
		return 0;
	}

	i = 0u;
	while (pfx[i] != '\0') {
		if (s[i] == '\0') {
			return 0;
		}
		if (b507_tolower_ascii(s[i]) != b507_tolower_ascii(pfx[i])) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_starts_with_ci(const char *s, const char *pfx)
    __attribute__((alias("gj_starts_with_ci")));
