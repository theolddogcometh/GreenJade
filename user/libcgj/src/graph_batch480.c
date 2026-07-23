/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch480: freestanding absolute-path predicate.
 *
 * Surface (unique symbols):
 *   int gj_path_is_abs(const char *path);
 *     — 1 if path is non-NULL and begins with '/'; 0 otherwise
 *       (NULL or relative). Unix-style only; no drive-letter / UNC.
 *   int __gj_path_is_abs  (alias)
 *   __libcgj_batch480_marker = "libcgj-batch480"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch480_marker[] = "libcgj-batch480";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_abs — absolute path predicate (leading '/').
 *
 * Returns:
 *   1  path non-NULL and path[0] == '/'
 *   0  path NULL or relative
 */
int
gj_path_is_abs(const char *path)
{
	if (path == NULL) {
		return 0;
	}
	return (path[0] == '/') ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_abs(const char *path)
    __attribute__((alias("gj_path_is_abs")));
