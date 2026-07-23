/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch684: freestanding relative-path predicate.
 *
 * Surface (unique symbols):
 *   int gj_path_is_rel(const char *p);
 *     — 1 if p is non-NULL and does not begin with '/'; 0 otherwise
 *       (NULL or absolute). Unix-style only; no drive-letter / UNC.
 *   int __gj_path_is_rel  (alias)
 *   __libcgj_batch684_marker = "libcgj-batch684"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch684_marker[] = "libcgj-batch684";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_rel — relative path predicate (not leading '/').
 *
 * Returns:
 *   1  p non-NULL and p[0] != '/'
 *   0  p NULL or absolute
 */
int
gj_path_is_rel(const char *p)
{
	if (p == NULL) {
		return 0;
	}
	return (p[0] == '/') ? 0 : 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_rel(const char *p)
    __attribute__((alias("gj_path_is_rel")));
