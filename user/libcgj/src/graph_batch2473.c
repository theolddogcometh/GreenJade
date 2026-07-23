/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2473: freestanding last path-separator index.
 *
 * Surface (unique symbols):
 *   size_t gj_path_last_sep_u(const char *path);
 *     — Return the 0-based byte index of the last directory separator
 *       ('/' or '\\') in path. If path is NULL, empty, or contains no
 *       separator, return (size_t)-1.
 *   size_t __gj_path_last_sep_u  (alias)
 *   __libcgj_batch2473_marker = "libcgj-batch2473"
 *
 * Path exclusive wave (2471–2480). Distinct from gj_path_basename_start
 * (batch1984, '/' only, returns 0 on miss) — this reports the separator
 * index itself and accepts both separators.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2473_marker[] = "libcgj-batch2473";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2473_is_sep(char ch)
{
	return (ch == '/' || ch == '\\') ? 1 : 0;
}

static size_t
b2473_last_sep(const char *szPath)
{
	size_t i;
	size_t iLast;

	if (szPath == NULL || szPath[0] == '\0') {
		return (size_t)-1;
	}

	iLast = (size_t)-1;
	for (i = 0u; szPath[i] != '\0'; i++) {
		if (b2473_is_sep(szPath[i])) {
			iLast = i;
		}
	}
	return iLast;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_last_sep_u — index of last '/' or '\\', or (size_t)-1.
 *
 * Examples:
 *   "a/b/c"   → 3
 *   "a\\b\\c" → 3
 *   "a/b\\c"  → 3
 *   "foo"     → (size_t)-1
 *   "/"       → 0
 *   NULL      → (size_t)-1
 */
size_t
gj_path_last_sep_u(const char *szPath)
{
	(void)NULL;
	return b2473_last_sep(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_last_sep_u(const char *szPath)
    __attribute__((alias("gj_path_last_sep_u")));
