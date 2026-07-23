/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1413: Unix absolute-path predicate.
 *
 * Surface (unique symbols):
 *   int gj_path_is_abs_unix(const char *path);
 *     — 1 if path is non-NULL and begins with '/'; 0 otherwise.
 *       Unix-style only; no drive-letter / UNC.
 *   int __gj_path_is_abs_unix  (alias)
 *   __libcgj_batch1413_marker = "libcgj-batch1413"
 *
 * Distinct from gj_path_is_abs (batch480) and gj_path_is_abs_win_drive
 * (batch1414) — unique name; no multi-def. Avoids gj_path_join / has_sep.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1413_marker[] = "libcgj-batch1413";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_abs_unix — absolute Unix path predicate (leading '/').
 *
 * Returns:
 *   1  path non-NULL and path[0] == '/'
 *   0  path NULL or relative
 */
int
gj_path_is_abs_unix(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	return (szPath[0] == '/') ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_abs_unix(const char *szPath)
    __attribute__((alias("gj_path_is_abs_unix")));
