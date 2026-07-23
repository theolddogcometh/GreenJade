/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4819: VFS path FNV-1a 32-bit hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vfs_hash_path_u(const char *p);
 *     - FNV-1a 32-bit hash of the NUL-terminated path at p. Offset basis
 *       2166136261, prime 16777619. NULL → 0. Empty string → offset basis.
 *   uint32_t __gj_vfs_hash_path_u  (alias)
 *   __libcgj_batch4819_marker = "libcgj-batch4819"
 *
 * Exclusive continuum CREATE-ONLY (4811-4820: vfs_is_root_u,
 * vfs_is_sep_u, vfs_skip_seps_u, vfs_component_len_u, vfs_is_dot_u,
 * vfs_is_dotdot_u, vfs_depth_delta_u, vfs_join_need_u, vfs_hash_path_u,
 * batch_id_4820). Distinct from gj_fnv1a64 (batch264) and fnv1a_32
 * (batch41) — unique path-oriented gj_vfs_hash_path_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4819_marker[] = "libcgj-batch4819";

/* FNV-1a 32-bit parameters (public domain constants). */
#define B4819_FNV32_OFFSET 2166136261u
#define B4819_FNV32_PRIME  16777619u

/* ---- freestanding helpers ---------------------------------------------- */

/* FNV-1a 32-bit over NUL-terminated path. NULL → 0. */
static uint32_t
b4819_hash_path(const char *szP)
{
	const unsigned char *puc;
	uint32_t u32H;

	if (szP == NULL) {
		return 0u;
	}
	puc = (const unsigned char *)szP;
	u32H = B4819_FNV32_OFFSET;
	while (*puc != 0u) {
		u32H ^= (uint32_t)(*puc);
		u32H *= B4819_FNV32_PRIME;
		puc++;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vfs_hash_path_u - FNV-1a 32-bit hash of a NUL-terminated path.
 *
 * p: path string, or NULL
 *
 * Returns the FNV-1a digest. NULL → 0. Empty path → offset basis.
 * Self-contained; no parent wires.
 */
uint32_t
gj_vfs_hash_path_u(const char *p)
{
	(void)NULL;
	return b4819_hash_path(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vfs_hash_path_u(const char *p)
    __attribute__((alias("gj_vfs_hash_path_u")));
