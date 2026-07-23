/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch508: freestanding case-insensitive suffix test.
 *
 * Surface (unique symbols):
 *   int gj_ends_with_ci(const char *s, const char *sfx);
 *     — 1 if s ends with sfx under ASCII case-insensitive compare,
 *       0 otherwise. NULL either argument → 0. Empty sfx → 1 (when s
 *       is non-NULL). ASCII A–Z folded to a–z only; no locale.
 *   int __gj_ends_with_ci  (alias)
 *   __libcgj_batch508_marker = "libcgj-batch508"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch508_marker[] = "libcgj-batch508";

/* ---- freestanding helpers ---------------------------------------------- */

static char
b508_tolower_ascii(char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return (char)(ch - 'A' + 'a');
	}
	return ch;
}

static size_t
b508_strlen(const char *s)
{
	size_t n;

	n = 0u;
	while (s[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ends_with_ci — case-insensitive ASCII suffix predicate.
 *
 * s:   NUL-terminated haystack (NULL → 0)
 * sfx: NUL-terminated suffix needle (NULL → 0)
 *
 * Edge cases:
 *   s == NULL or sfx == NULL → 0
 *   sfx empty ("")           → 1 (every string ends with the empty suffix)
 *   s shorter than sfx       → 0
 *   "FooBar" / "bar"         → 1
 *   "FooBar" / "BAR"         → 1
 *   "FooBar" / "baz"         → 0
 *
 * Does not call libc. Returns 1 on match, 0 otherwise.
 */
int
gj_ends_with_ci(const char *s, const char *sfx)
{
	size_t n_s;
	size_t n_sfx;
	size_t i;
	size_t base;

	if (s == NULL || sfx == NULL) {
		return 0;
	}

	n_s = b508_strlen(s);
	n_sfx = b508_strlen(sfx);

	if (n_sfx == 0u) {
		return 1;
	}
	if (n_sfx > n_s) {
		return 0;
	}

	base = n_s - n_sfx;
	for (i = 0u; i < n_sfx; i++) {
		if (b508_tolower_ascii(s[base + i]) !=
		    b508_tolower_ascii(sfx[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ends_with_ci(const char *s, const char *sfx)
    __attribute__((alias("gj_ends_with_ci")));
