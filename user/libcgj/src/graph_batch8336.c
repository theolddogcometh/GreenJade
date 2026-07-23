/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8336: path is-dot component probe (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_dot_u_8336(const char *path);
 *     - Return 1 if path is exactly the single-dot component ".";
 *       else 0 (including NULL). Component-level exact match only.
 *   int __gj_path_dot_u_8336  (alias)
 *   __libcgj_batch8336_marker = "libcgj-batch8336"
 *
 * Env/path probe stubs wave (8331-8340). Distinct from gj_path_is_dot
 * (batch918) and gj_path_is_dot_u (batch2825) — unique _u_8336
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8336_marker[] = "libcgj-batch8336";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff szPath is exactly ".". */
static int
b8336_dot(const char *szPath)
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
 * gj_path_dot_u_8336 - true iff path is exactly ".".
 *
 * path: NUL-terminated path component, or NULL
 *
 * Returns 1 on exact match, else 0. Does not accept "./" or longer forms.
 */
int
gj_path_dot_u_8336(const char *szPath)
{
	(void)NULL;
	return b8336_dot(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_dot_u_8336(const char *szPath)
    __attribute__((alias("gj_path_dot_u_8336")));
