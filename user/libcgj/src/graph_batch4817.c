/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4817: VFS path depth delta for one component.
 *
 * Surface (unique symbols):
 *   int gj_vfs_depth_delta_u(const char *comp, size_t n);
 *     - Classify an n-byte path component for lexical depth accounting:
 *         ".." → -1  (climb)
 *         "."  →  0  (no-op)
 *         empty / NULL → 0
 *         otherwise → +1  (enter)
 *       Length-bounded; does not require a trailing NUL in the span.
 *   int __gj_vfs_depth_delta_u  (alias)
 *   __libcgj_batch4817_marker = "libcgj-batch4817"
 *
 * Exclusive continuum CREATE-ONLY (4811-4820: vfs_is_root_u,
 * vfs_is_sep_u, vfs_skip_seps_u, vfs_component_len_u, vfs_is_dot_u,
 * vfs_is_dotdot_u, vfs_depth_delta_u, vfs_join_need_u, vfs_hash_path_u,
 * batch_id_4820). Distinct from gj_json_depth_delta_u (batch3884 JSON
 * structural) — unique path-component gj_vfs_depth_delta_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4817_marker[] = "libcgj-batch4817";

/* ---- freestanding helpers ---------------------------------------------- */

/* Depth delta for one component: +1 / 0 / -1. */
static int
b4817_depth_delta(const char *szComp, size_t n)
{
	if (szComp == NULL || n == 0u) {
		return 0;
	}
	if (n == 1u && szComp[0] == '.') {
		return 0;
	}
	if (n == 2u && szComp[0] == '.' && szComp[1] == '.') {
		return -1;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vfs_depth_delta_u - lexical depth change for one path component.
 *
 * comp: component bytes (may be unterminated within a larger path)
 * n:    byte length of the span
 *
 * Returns +1 for an ordinary name, 0 for "." / empty / NULL, -1 for "..".
 * Useful when normalizing paths without writing a buffer.
 * Self-contained; no parent wires.
 */
int
gj_vfs_depth_delta_u(const char *comp, size_t n)
{
	(void)NULL;
	return b4817_depth_delta(comp, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_vfs_depth_delta_u(const char *comp, size_t n)
    __attribute__((alias("gj_vfs_depth_delta_u")));
