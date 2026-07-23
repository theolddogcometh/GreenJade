/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch683: freestanding path-separator predicate.
 *
 * Surface (unique symbols):
 *   int gj_path_has_sep(const char *p);
 *     — 1 if p is non-NULL and contains '/' or '\\'; 0 otherwise
 *       (NULL or no separator). Scans the whole string; both Unix and
 *       Windows-style separators count.
 *   int __gj_path_has_sep  (alias)
 *   __libcgj_batch683_marker = "libcgj-batch683"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch683_marker[] = "libcgj-batch683";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_has_sep — path contains a directory separator.
 *
 * Returns:
 *   1  p non-NULL and contains '/' or '\\'
 *   0  p NULL or no separator present
 */
int
gj_path_has_sep(const char *p)
{
	const char *s;

	if (p == NULL) {
		return 0;
	}
	for (s = p; *s != '\0'; s++) {
		if (*s == '/' || *s == '\\') {
			return 1;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_sep(const char *p)
    __attribute__((alias("gj_path_has_sep")));
