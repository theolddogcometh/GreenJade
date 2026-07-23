/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2365: Steam root path soft-ok predicate
 * (post-2360 product path exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_steam_root_ok(const char *path);
 *     - Return 1 if path is a non-NULL absolute path of length >= 2
 *       (leading '/' plus at least one more character). Soft product
 *       Steam root shape check only; does not resolve or scan for a
 *       "steam" component (see batch1865/1866 path component helpers).
 *   int __gj_steam_root_ok  (alias)
 *   __libcgj_batch2365_marker = "libcgj-batch2365"
 *
 * Post-2360 product path exclusive wave (2361-2370). Distinct from
 * gj_steam_path_is_compatdata (batch1865), gj_steam_path_is_shadercache
 * (batch1866), and gj_path_is_abs (batch480) - unique steam_root_ok
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2365_marker[] = "libcgj-batch2365";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if path looks like a usable absolute Steam install root:
 * leading '/', at least one more non-NUL character.
 */
static int
b2365_steam_root_ok(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	if (szPath[0] != '/') {
		return 0;
	}
	if (szPath[1] == '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_root_ok - soft check for a Steam root install path.
 *
 * path: NUL-terminated absolute path (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and has length >= 2, else 0.
 * Does not call libc or touch the filesystem.
 */
int
gj_steam_root_ok(const char *szPath)
{
	(void)NULL;
	return b2365_steam_root_ok(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_steam_root_ok(const char *szPath)
    __attribute__((alias("gj_steam_root_ok")));
