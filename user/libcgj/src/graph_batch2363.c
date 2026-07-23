/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2363: rootfs path soft-ok predicate
 * (post-2360 product path exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_rootfs_path_ok(const char *path);
 *     - Return 1 if path is a non-NULL absolute Unix rootfs path:
 *       begins with '/', not only empty after the leading slash
 *       requirement ( "/" alone is accepted). Rejects relative and
 *       NULL. Soft path-shape check; no filesystem access.
 *   int __gj_rootfs_path_ok  (alias)
 *   __libcgj_batch2363_marker = "libcgj-batch2363"
 *
 * Post-2360 product path exclusive wave (2361-2370). Distinct from
 * gj_path_is_abs (batch480) and gj_install_prefix_abs_p (batch1954) -
 * unique gj_rootfs_path_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2363_marker[] = "libcgj-batch2363";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if path is an absolute rootfs path shape.
 * Accepts "/" and longer absolute paths; rejects NULL and relative.
 */
static int
b2363_rootfs_ok(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	if (szPath[0] != '/') {
		return 0;
	}
	/* Leading slash is enough; "/" alone is a valid rootfs root. */
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rootfs_path_ok - soft check for a product rootfs path.
 *
 * path: NUL-terminated path (NULL -> 0; relative -> 0)
 *
 * Returns 1 when path begins with '/', else 0. Does not call libc or
 * touch the filesystem.
 */
int
gj_rootfs_path_ok(const char *szPath)
{
	(void)NULL;
	return b2363_rootfs_ok(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rootfs_path_ok(const char *szPath)
    __attribute__((alias("gj_rootfs_path_ok")));
