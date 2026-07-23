/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2163: freestanding path is-root predicate.
 *
 * Surface (unique symbols):
 *   int gj_path_is_root_p(const char *path);
 *     - Return 1 if path is exactly the Unix root "/"; else 0
 *       (including NULL, empty, relative, or multi-slash forms).
 *   int __gj_path_is_root_p  (alias)
 *   __libcgj_batch2163_marker = "libcgj-batch2163"
 *
 * Post-2160 path exclusive wave (2161-2170). Distinct from
 * gj_path_is_abs (batch480) which accepts any leading '/'. Unique
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2163_marker[] = "libcgj-batch2163";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2163_is_root_p(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	if (szPath[0] != '/') {
		return 0;
	}
	if (szPath[1] != '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_root_p - true iff path is exactly "/".
 *
 * path: NUL-terminated path, or NULL
 *
 * Returns 1 on exact root match, else 0. Does not treat "//" or "///"
 * as root (exact single-slash only).
 */
int
gj_path_is_root_p(const char *path)
{
	(void)NULL;
	return b2163_is_root_p(path);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_root_p(const char *path)
    __attribute__((alias("gj_path_is_root_p")));
