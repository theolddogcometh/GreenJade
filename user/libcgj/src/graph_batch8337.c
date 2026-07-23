/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8337: path is-dotdot component probe (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_dotdot_u_8337(const char *path);
 *     - Return 1 if path is exactly the parent-dot component "..";
 *       else 0 (including NULL). Component-level exact match only.
 *   int __gj_path_dotdot_u_8337  (alias)
 *   __libcgj_batch8337_marker = "libcgj-batch8337"
 *
 * Env/path probe stubs wave (8331-8340). Distinct from
 * gj_path_is_dotdot (batch919) and gj_path_is_dotdot_u (batch2826) —
 * unique _u_8337 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8337_marker[] = "libcgj-batch8337";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff szPath is exactly "..". */
static int
b8337_dotdot(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	if (szPath[0] != '.') {
		return 0;
	}
	if (szPath[1] != '.') {
		return 0;
	}
	if (szPath[2] != '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_dotdot_u_8337 - true iff path is exactly "..".
 *
 * path: NUL-terminated path component, or NULL
 *
 * Returns 1 on exact match, else 0. Does not accept "../" or longer forms.
 */
int
gj_path_dotdot_u_8337(const char *szPath)
{
	(void)NULL;
	return b8337_dotdot(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_dotdot_u_8337(const char *szPath)
    __attribute__((alias("gj_path_dotdot_u_8337")));
