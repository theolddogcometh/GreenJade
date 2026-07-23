/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4814: VFS path component length probe.
 *
 * Surface (unique symbols):
 *   size_t gj_vfs_component_len_u(const char *p);
 *     - Return the byte length of the current path component starting
 *       at p: count non-'/' bytes until '/' or NUL. Does not skip
 *       leading separators (a leading '/' yields length 0). NULL → 0.
 *   size_t __gj_vfs_component_len_u  (alias)
 *   __libcgj_batch4814_marker = "libcgj-batch4814"
 *
 * Exclusive continuum CREATE-ONLY (4811-4820: vfs_is_root_u,
 * vfs_is_sep_u, vfs_skip_seps_u, vfs_component_len_u, vfs_is_dot_u,
 * vfs_is_dotdot_u, vfs_depth_delta_u, vfs_join_need_u, vfs_hash_path_u,
 * batch_id_4820). Distinct from gj_path_next_seg (batch2165) which
 * also skips leading seps — unique length-only gj_vfs_component_len_u
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4814_marker[] = "libcgj-batch4814";

/* ---- freestanding helpers ---------------------------------------------- */

/* Length of component at szP (until '/' or NUL). */
static size_t
b4814_component_len(const char *szP)
{
	size_t i;

	if (szP == NULL) {
		return 0u;
	}
	i = 0u;
	while (szP[i] != '\0' && szP[i] != '/') {
		i++;
	}
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vfs_component_len_u - length of the path component starting at p.
 *
 * p: pointer into a path (component start), or NULL
 *
 * Returns the number of bytes until the next '/' or terminating NUL.
 * Leading '/' is not skipped: component length of "/" is 0.
 * Self-contained; no parent wires.
 */
size_t
gj_vfs_component_len_u(const char *p)
{
	(void)NULL;
	return b4814_component_len(p);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_vfs_component_len_u(const char *p)
    __attribute__((alias("gj_vfs_component_len_u")));
