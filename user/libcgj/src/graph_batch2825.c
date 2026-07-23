/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2825: freestanding path is-dot predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_is_dot_u(const char *path);
 *     - Return 1 if path is exactly the single-dot component ".";
 *       else 0 (including NULL). Component-level exact match only.
 *   int __gj_path_is_dot_u  (alias)
 *   __libcgj_batch2825_marker = "libcgj-batch2825"
 *
 * ASCII/path exclusive wave (2821-2830). Distinct from gj_path_is_dot
 * (batch918) and gj_path_is_dot_p (batch2161) — unique _u surface; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2825_marker[] = "libcgj-batch2825";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff szPath is exactly ".". */
static int
b2825_is_dot(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	if (szPath[0] != '.') {
		return 0;
	}
	if (szPath[1] != '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_dot_u - true iff path is exactly ".".
 *
 * path: NUL-terminated path component, or NULL
 *
 * Returns 1 on exact match, else 0. Does not accept "./" or longer forms.
 */
int
gj_path_is_dot_u(const char *szPath)
{
	(void)NULL;
	return b2825_is_dot(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_dot_u(const char *szPath)
    __attribute__((alias("gj_path_is_dot_u")));
