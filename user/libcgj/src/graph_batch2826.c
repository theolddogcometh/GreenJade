/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2826: freestanding path is-dotdot predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_is_dotdot_u(const char *path);
 *     - Return 1 if path is exactly the parent-dot component "..";
 *       else 0 (including NULL). Component-level exact match only.
 *   int __gj_path_is_dotdot_u  (alias)
 *   __libcgj_batch2826_marker = "libcgj-batch2826"
 *
 * ASCII/path exclusive wave (2821-2830). Distinct from gj_path_is_dotdot
 * (batch919) and gj_path_is_dotdot_p (batch2162) — unique _u surface;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2826_marker[] = "libcgj-batch2826";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff szPath is exactly "..". */
static int
b2826_is_dotdot(const char *szPath)
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
 * gj_path_is_dotdot_u - true iff path is exactly "..".
 *
 * path: NUL-terminated path component, or NULL
 *
 * Returns 1 on exact match, else 0. Does not accept "../" or longer forms.
 */
int
gj_path_is_dotdot_u(const char *szPath)
{
	(void)NULL;
	return b2826_is_dotdot(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_dotdot_u(const char *szPath)
    __attribute__((alias("gj_path_is_dotdot_u")));
