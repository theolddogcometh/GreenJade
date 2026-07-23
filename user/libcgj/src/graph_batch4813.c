/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4813: VFS skip consecutive path separators.
 *
 * Surface (unique symbols):
 *   size_t gj_vfs_skip_seps_u(const char *p);
 *     - Return the byte offset past consecutive leading '/' separators
 *       at p. Offset is relative to p. NULL p returns 0. Paths with no
 *       leading slash return 0. Does not allocate.
 *   size_t __gj_vfs_skip_seps_u  (alias)
 *   __libcgj_batch4813_marker = "libcgj-batch4813"
 *
 * Exclusive continuum CREATE-ONLY (4811-4820: vfs_is_root_u,
 * vfs_is_sep_u, vfs_skip_seps_u, vfs_component_len_u, vfs_is_dot_u,
 * vfs_is_dotdot_u, vfs_depth_delta_u, vfs_join_need_u, vfs_hash_path_u,
 * batch_id_4820). Distinct from gj_path_skip_slashes (batch2164 pointer
 * form) — unique offset-returning gj_vfs_skip_seps_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4813_marker[] = "libcgj-batch4813";

/* ---- freestanding helpers ---------------------------------------------- */

/* Byte offset past leading '/'. */
static size_t
b4813_skip_seps(const char *szP)
{
	size_t i;

	if (szP == NULL) {
		return 0u;
	}
	i = 0u;
	while (szP[i] == '/') {
		i++;
	}
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vfs_skip_seps_u - offset past consecutive leading '/' characters.
 *
 * p: NUL-terminated path, or NULL
 *
 * Returns the number of leading '/' bytes skipped (0 when p is NULL or
 * does not start with '/'). Caller may use p + result as the residual.
 * Self-contained; no parent wires.
 */
size_t
gj_vfs_skip_seps_u(const char *p)
{
	(void)NULL;
	return b4813_skip_seps(p);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_vfs_skip_seps_u(const char *p)
    __attribute__((alias("gj_vfs_skip_seps_u")));
