/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1981: freestanding path ".." component probe.
 *
 * Surface (unique symbols):
 *   int gj_path_has_dotdot(const char *szPath);
 *     — Return 1 if szPath contains a path component that is exactly
 *       ".." (dot-dot), else 0. Component boundaries are '/'. A bare
 *       ".." matches; so do "/a/../b", "../x", and "x/..". Names that
 *       merely embed the two-dot substring (e.g. "foo..bar", "..x",
 *       "x..") do not match. NULL or empty → 0.
 *   int __gj_path_has_dotdot  (alias)
 *   __libcgj_batch1981_marker = "libcgj-batch1981"
 *
 * String path product exclusive wave (1981–1990). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch1981_marker[] = "libcgj-batch1981";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if the component [iStart, iEnd) is exactly ".." (two chars).
 */
static int
b1981_is_dotdot(const char *szPath, size_t iStart, size_t iEnd)
{
	if (iEnd != iStart + 2u) {
		return 0;
	}
	if (szPath[iStart] != '.' || szPath[iStart + 1u] != '.') {
		return 0;
	}
	return 1;
}

/* Scan slash-separated components for an exact ".." segment. */
static int
b1981_scan(const char *szPath)
{
	size_t i;
	size_t iStart;

	if (szPath == NULL || szPath[0] == '\0') {
		return 0;
	}

	iStart = 0u;
	for (i = 0u; ; i++) {
		if (szPath[i] == '/' || szPath[i] == '\0') {
			if (i > iStart && b1981_is_dotdot(szPath, iStart, i)) {
				return 1;
			}
			if (szPath[i] == '\0') {
				break;
			}
			iStart = i + 1u;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_has_dotdot — 1 if path has a ".." component.
 *
 * szPath: NUL-terminated path, or NULL
 * Returns 1 on match, else 0.
 */
int
gj_path_has_dotdot(const char *szPath)
{
	(void)NULL;
	return b1981_scan(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_dotdot(const char *szPath)
    __attribute__((alias("gj_path_has_dotdot")));
