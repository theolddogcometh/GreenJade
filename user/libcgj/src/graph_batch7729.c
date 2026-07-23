/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7729: chdir path request validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chdir_path_req_ok_u_7729(uint32_t len);
 *     - Return 1 if path is nonempty and len < PATH_MAX, else 0.
 *   uint32_t __gj_chdir_path_req_ok_u_7729  (alias)
 *   __libcgj_batch7729_marker = "libcgj-batch7729"
 *
 * Exclusive continuum CREATE-ONLY (7721-7730: chdir path length stubs —
 * path_max, path_len_ok, nonempty, name_max, comp_len_ok, is_abs,
 * is_rel, path_room, req_ok, continuum + batch_id_7730). Unique
 * gj_chdir_path_req_ok_u_7729 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7729_marker[] = "libcgj-batch7729";

/* Soft Linux PATH_MAX for chdir path request. */
#define B7729_PATH_MAX  ((uint32_t)4096u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7729_path_req_ok(uint32_t u32Len)
{
	if (u32Len == 0u)
		return 0u;
	if (u32Len >= B7729_PATH_MAX)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chdir_path_req_ok_u_7729 - 1 if soft chdir path request is length-ok.
 *
 * len: soft path length in bytes (excluding trailing NUL)
 *
 * Returns 1 when 0 < len < 4096, else 0. Combines nonempty + PATH_MAX
 * soft policy without calling sibling helpers. No parent wires. Does
 * not call chdir.
 */
uint32_t
gj_chdir_path_req_ok_u_7729(uint32_t u32Len)
{
	(void)NULL;
	return b7729_path_req_ok(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chdir_path_req_ok_u_7729(uint32_t u32Len)
    __attribute__((alias("gj_chdir_path_req_ok_u_7729")));
