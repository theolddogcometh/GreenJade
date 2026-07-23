/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8339: path NULL probe stub (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_nul_u_8339(const char *path);
 *     - Return 1 if path is NULL; else 0. Pure nullness probe for
 *       path pointers before abs/rel/dot checks. Does not inspect
 *       path contents when non-NULL.
 *   int __gj_path_nul_u_8339  (alias)
 *   __libcgj_batch8339_marker = "libcgj-batch8339"
 *
 * Env/path probe stubs wave (8331-8340). Distinct from empty-string
 * and abs/rel probes — unique _u_8339 surface; no multi-def. No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8339_marker[] = "libcgj-batch8339";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff szPath is NULL. */
static int
b8339_nul(const char *szPath)
{
	if (szPath == NULL) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_nul_u_8339 - 1 if path is NULL, else 0.
 *
 * path: path pointer to probe (may be NULL)
 *
 * Does not dereference path. No parent wires.
 */
int
gj_path_nul_u_8339(const char *szPath)
{
	(void)NULL;
	return b8339_nul(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_nul_u_8339(const char *szPath)
    __attribute__((alias("gj_path_nul_u_8339")));
