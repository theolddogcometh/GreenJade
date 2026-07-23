/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4815: VFS length-bounded is-dot predicate.
 *
 * Surface (unique symbols):
 *   int gj_vfs_is_dot_u(const char *p, size_t n);
 *     - Return 1 if the n-byte span at p is exactly the single-dot
 *       component "."; else 0 (including NULL p with n > 0, or n != 1).
 *       Length-bounded: does not require a trailing NUL inside the span.
 *   int __gj_vfs_is_dot_u  (alias)
 *   __libcgj_batch4815_marker = "libcgj-batch4815"
 *
 * Exclusive continuum CREATE-ONLY (4811-4820: vfs_is_root_u,
 * vfs_is_sep_u, vfs_skip_seps_u, vfs_component_len_u, vfs_is_dot_u,
 * vfs_is_dotdot_u, vfs_depth_delta_u, vfs_join_need_u, vfs_hash_path_u,
 * batch_id_4820). Distinct from gj_path_is_dot_u (batch2825 NUL form)
 * and gj_path_is_dot_u2 (batch4432) — unique length-bounded
 * gj_vfs_is_dot_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4815_marker[] = "libcgj-batch4815";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff the n-byte span at szP is exactly ".". */
static int
b4815_is_dot(const char *szP, size_t n)
{
	if (szP == NULL) {
		return 0;
	}
	if (n != 1u) {
		return 0;
	}
	if (szP[0] != '.') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vfs_is_dot_u - true iff the n-byte span at p is exactly ".".
 *
 * p: component bytes (may be unterminated within a larger path)
 * n: byte length of the span
 *
 * Returns 1 on exact match, else 0. Does not accept "./" or longer forms.
 * Self-contained; no parent wires.
 */
int
gj_vfs_is_dot_u(const char *p, size_t n)
{
	(void)NULL;
	return b4815_is_dot(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_vfs_is_dot_u(const char *p, size_t n)
    __attribute__((alias("gj_vfs_is_dot_u")));
