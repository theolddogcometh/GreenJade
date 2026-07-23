/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2824: freestanding absolute-path predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_is_abs_u(const char *path);
 *     - Return 1 if path is non-NULL and begins with '/'; else 0
 *       (NULL or relative). Unix-style only; no drive-letter / UNC.
 *   int __gj_path_is_abs_u  (alias)
 *   __libcgj_batch2824_marker = "libcgj-batch2824"
 *
 * ASCII/path exclusive wave (2821-2830). Distinct from gj_path_is_abs
 * (batch480), gj_path_is_abs_unix (batch1413), and
 * gj_path_is_abs_win_drive (batch1414) — unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2824_marker[] = "libcgj-batch2824";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff szPath is non-NULL and begins with '/'. */
static int
b2824_is_abs(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	if (szPath[0] == '/') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_abs_u - absolute Unix path predicate (leading '/').
 *
 * path: NUL-terminated path, or NULL
 *
 * Returns 1 when path is non-NULL and path[0] == '/', else 0.
 * Does not accept Windows drive-letter or UNC forms.
 */
int
gj_path_is_abs_u(const char *szPath)
{
	(void)NULL;
	return b2824_is_abs(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_abs_u(const char *szPath)
    __attribute__((alias("gj_path_is_abs_u")));
