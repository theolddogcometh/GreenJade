/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4812: VFS path-separator char probe (_u).
 *
 * Surface (unique symbols):
 *   int gj_vfs_is_sep_u(unsigned c);
 *     - Return 1 if c is the Unix directory separator '/', else 0.
 *       VFS continuum is POSIX-only (no backslash). Full unsigned
 *       character range accepted; only the exact byte 0x2F matches.
 *   int __gj_vfs_is_sep_u  (alias)
 *   __libcgj_batch4812_marker = "libcgj-batch4812"
 *
 * Exclusive continuum CREATE-ONLY (4811-4820: vfs_is_root_u,
 * vfs_is_sep_u, vfs_skip_seps_u, vfs_component_len_u, vfs_is_dot_u,
 * vfs_is_dotdot_u, vfs_depth_delta_u, vfs_join_need_u, vfs_hash_path_u,
 * batch_id_4820). Distinct from gj_path_is_sep_u (batch2471 dual-sep)
 * and gj_path_is_sep_u2 (batch3484) — unique gj_vfs_is_sep_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4812_marker[] = "libcgj-batch4812";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff uC is '/'. */
static int
b4812_is_sep(unsigned uC)
{
	if (uC == (unsigned)'/') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vfs_is_sep_u - 1 if c is '/', else 0.
 *
 * c: character code as unsigned (full range accepted)
 *
 * Returns 1 on Unix separator match, 0 otherwise. Does not match '\\'.
 * Self-contained; no parent wires.
 */
int
gj_vfs_is_sep_u(unsigned uC)
{
	(void)NULL;
	return b4812_is_sep(uC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_vfs_is_sep_u(unsigned uC)
    __attribute__((alias("gj_vfs_is_sep_u")));
