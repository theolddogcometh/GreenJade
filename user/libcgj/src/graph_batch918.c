/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch918: path component is "." predicate.
 *
 * Surface (unique symbols):
 *   int gj_path_is_dot(const char *path);
 *     — Return 1 if path is exactly the single-dot component ".";
 *       else 0 (including NULL).
 *   int __gj_path_is_dot  (alias)
 *   __libcgj_batch918_marker = "libcgj-batch918"
 *
 * Does NOT redefine gj_path_is_abs / gj_path_normalize — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch918_marker[] = "libcgj-batch918";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_dot — true iff path is exactly ".".
 *
 * Does not accept "./" or longer forms; component-level exact match only.
 */
int
gj_path_is_dot(const char *path)
{
	if (path == NULL) {
		return 0;
	}
	return (path[0] == '.' && path[1] == '\0') ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_dot(const char *path)
    __attribute__((alias("gj_path_is_dot")));
