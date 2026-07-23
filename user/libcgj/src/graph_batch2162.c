/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2162: freestanding path is-dotdot predicate.
 *
 * Surface (unique symbols):
 *   int gj_path_is_dotdot_p(const char *path);
 *     - Return 1 if path is exactly the parent component "..";
 *       else 0 (including NULL). Component-level exact match only.
 *   int __gj_path_is_dotdot_p  (alias)
 *   __libcgj_batch2162_marker = "libcgj-batch2162"
 *
 * Post-2160 path exclusive wave (2161-2170). Pair of gj_path_is_dot_p
 * (batch2161). Distinct from gj_path_is_dotdot (batch919) - unique _p
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2162_marker[] = "libcgj-batch2162";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2162_is_dotdot_p(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	if (szPath[0] != '.' || szPath[1] != '.') {
		return 0;
	}
	if (szPath[2] != '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_dotdot_p - true iff path is exactly "..".
 *
 * path: NUL-terminated path component, or NULL
 *
 * Returns 1 on exact match, else 0. Does not accept "../" or longer forms.
 */
int
gj_path_is_dotdot_p(const char *path)
{
	(void)NULL;
	return b2162_is_dotdot_p(path);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_dotdot_p(const char *path)
    __attribute__((alias("gj_path_is_dotdot_p")));
