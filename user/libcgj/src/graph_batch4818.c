/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4818: VFS two-length path join byte need.
 *
 * Surface (unique symbols):
 *   size_t gj_vfs_join_need_u(size_t a_len, size_t b_len);
 *     - Byte capacity needed (including trailing NUL) to join two path
 *       fragments of lengths a_len and b_len with a single '/' when both
 *       are non-empty. Empty sides contribute no separator:
 *         (0,0) → 1  ("" + NUL)
 *         (a,0) → a + 1
 *         (0,b) → b + 1
 *         (a,b) → a + 1 + b + 1
 *       On size_t overflow of the sum, returns 0 (caller treats as fail).
 *   size_t __gj_vfs_join_need_u  (alias)
 *   __libcgj_batch4818_marker = "libcgj-batch4818"
 *
 * Exclusive continuum CREATE-ONLY (4811-4820: vfs_is_root_u,
 * vfs_is_sep_u, vfs_skip_seps_u, vfs_component_len_u, vfs_is_dot_u,
 * vfs_is_dotdot_u, vfs_depth_delta_u, vfs_join_need_u, vfs_hash_path_u,
 * batch_id_4820). Distinct from gj_path_join_need_u (batch2828 string
 * args) and gj_str_join_need_u (batch3634) — unique length-arg
 * gj_vfs_join_need_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4818_marker[] = "libcgj-batch4818";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Bytes needed (incl. trailing NUL) to join lengths nA and nB with '/'.
 * Returns 0 if the arithmetic would overflow size_t.
 */
static size_t
b4818_join_need(size_t nA, size_t nB)
{
	size_t nOut;

	if (nA == 0u && nB == 0u) {
		return 1u; /* "" + NUL */
	}
	if (nA == 0u) {
		if (nB == (size_t)-1) {
			return 0u;
		}
		return nB + 1u;
	}
	if (nB == 0u) {
		if (nA == (size_t)-1) {
			return 0u;
		}
		return nA + 1u;
	}

	/* nA + nB + 1 (sep) + 1 (NUL) */
	if (nA > (size_t)-1 - nB) {
		return 0u;
	}
	nOut = nA + nB;
	if (nOut > (size_t)-1 - 2u) {
		return 0u;
	}
	return nOut + 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vfs_join_need_u - bytes needed (incl. NUL) for a two-length join.
 *
 * a_len: length of the first fragment (bytes, not including NUL)
 * b_len: length of the second fragment (bytes, not including NUL)
 *
 * Examples (return includes trailing NUL):
 *   (1,1) → 4  ("a/b" + NUL when a_len=b_len=1)
 *   (0,0) → 1  ("" + NUL)
 *   (3,0) → 4  (three bytes + NUL)
 *   (0,5) → 6  (five bytes + NUL)
 *
 * Length-only; does not write a buffer. Returns 0 on size_t overflow.
 * Self-contained; no parent wires.
 */
size_t
gj_vfs_join_need_u(size_t a_len, size_t b_len)
{
	(void)NULL;
	return b4818_join_need(a_len, b_len);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_vfs_join_need_u(size_t a_len, size_t b_len)
    __attribute__((alias("gj_vfs_join_need_u")));
