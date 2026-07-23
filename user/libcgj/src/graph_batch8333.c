/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8333: absolute path probe stub (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_abs_u_8333(const char *path);
 *     - Return 1 if path is non-NULL and begins with '/'; else 0
 *       (NULL or relative). Unix-style only; no drive-letter / UNC.
 *   int __gj_path_abs_u_8333  (alias)
 *   __libcgj_batch8333_marker = "libcgj-batch8333"
 *
 * Env/path probe stubs wave (8331-8340). Distinct from gj_path_is_abs
 * (batch480), gj_path_is_abs_u (batch2824), and gj_path_is_abs_unix —
 * unique _u_8333 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8333_marker[] = "libcgj-batch8333";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff szPath is non-NULL and begins with '/'. */
static int
b8333_abs(const char *szPath)
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
 * gj_path_abs_u_8333 - absolute Unix path probe (leading '/').
 *
 * path: NUL-terminated path, or NULL
 *
 * Returns 1 when path is non-NULL and path[0] == '/', else 0.
 */
int
gj_path_abs_u_8333(const char *szPath)
{
	(void)NULL;
	return b8333_abs(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_abs_u_8333(const char *szPath)
    __attribute__((alias("gj_path_abs_u_8333")));
