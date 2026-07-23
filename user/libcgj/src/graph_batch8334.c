/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8334: relative path probe stub (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_rel_u_8334(const char *path);
 *     - Return 1 if path is non-NULL and does not begin with '/';
 *       else 0 (NULL or absolute). Unix-style only; empty string is
 *       treated as relative. No drive-letter / UNC handling.
 *   int __gj_path_rel_u_8334  (alias)
 *   __libcgj_batch8334_marker = "libcgj-batch8334"
 *
 * Env/path probe stubs wave (8331-8340). Complement of
 * gj_path_abs_u_8333. Distinct from gj_path_is_abs / _u — unique
 * _u_8334 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8334_marker[] = "libcgj-batch8334";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff szPath is non-NULL and does not begin with '/'. */
static int
b8334_rel(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	if (szPath[0] == '/') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_rel_u_8334 - relative Unix path probe (no leading '/').
 *
 * path: NUL-terminated path, or NULL
 *
 * Returns 1 when path is non-NULL and path[0] != '/', else 0.
 */
int
gj_path_rel_u_8334(const char *szPath)
{
	(void)NULL;
	return b8334_rel(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_rel_u_8334(const char *szPath)
    __attribute__((alias("gj_path_rel_u_8334")));
