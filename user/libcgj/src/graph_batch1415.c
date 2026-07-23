/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1415: path segment count.
 *
 * Surface (unique symbols):
 *   size_t gj_path_count_segs(const char *path);
 *     — Count non-empty path segments separated by '/'. Consecutive
 *       separators do not produce empty segments. Trailing separator
 *       does not add a segment. NULL / empty → 0.
 *   size_t __gj_path_count_segs  (alias)
 *   __libcgj_batch1415_marker = "libcgj-batch1415"
 *
 * Distinct from gj_path_join / gj_path_has_sep — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1415_marker[] = "libcgj-batch1415";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_count_segs — number of non-empty '/' segments.
 *
 * Examples:
 *   "a/b/c"   → 3
 *   "/a/b"    → 2
 *   "/"       → 0
 *   "a//b"    → 2
 *   "a/"      → 1
 *   "file"    → 1
 *   NULL / "" → 0
 */
size_t
gj_path_count_segs(const char *szPath)
{
	size_t nSegs;
	int fInSeg;
	size_t i;

	if (szPath == NULL || szPath[0] == '\0') {
		return 0u;
	}

	nSegs = 0u;
	fInSeg = 0;
	for (i = 0u; szPath[i] != '\0'; i++) {
		if (szPath[i] == '/') {
			fInSeg = 0;
		} else if (!fInSeg) {
			fInSeg = 1;
			nSegs++;
		}
	}
	return nSegs;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_count_segs(const char *szPath)
    __attribute__((alias("gj_path_count_segs")));
