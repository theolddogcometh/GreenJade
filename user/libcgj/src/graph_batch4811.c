/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4811: VFS path is-root predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_vfs_is_root_u(const char *p);
 *     - Return 1 if p is exactly the Unix root path "/"; else 0
 *       (including NULL, empty, relative, or multi-slash forms).
 *   int __gj_vfs_is_root_u  (alias)
 *   __libcgj_batch4811_marker = "libcgj-batch4811"
 *
 * Exclusive continuum CREATE-ONLY (4811-4820: vfs_is_root_u,
 * vfs_is_sep_u, vfs_skip_seps_u, vfs_component_len_u, vfs_is_dot_u,
 * vfs_is_dotdot_u, vfs_depth_delta_u, vfs_join_need_u, vfs_hash_path_u,
 * batch_id_4820). Distinct from gj_path_is_root_p (batch2163) —
 * unique gj_vfs_is_root_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4811_marker[] = "libcgj-batch4811";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff szP is exactly "/". */
static int
b4811_is_root(const char *szP)
{
	if (szP == NULL) {
		return 0;
	}
	if (szP[0] != '/') {
		return 0;
	}
	if (szP[1] != '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vfs_is_root_u - true iff path is exactly "/".
 *
 * p: NUL-terminated path, or NULL
 *
 * Returns 1 on exact root match, else 0. Does not treat "//" or "///"
 * as root (exact single-slash only). Self-contained; no parent wires.
 */
int
gj_vfs_is_root_u(const char *p)
{
	(void)NULL;
	return b4811_is_root(p);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_vfs_is_root_u(const char *p)
    __attribute__((alias("gj_vfs_is_root_u")));
