/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7725: chdir path component length stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chdir_comp_len_ok_u_7725(uint32_t len);
 *     - Return 1 if component len is in (0, NAME_MAX], else 0.
 *   uint32_t __gj_chdir_comp_len_ok_u_7725  (alias)
 *   __libcgj_batch7725_marker = "libcgj-batch7725"
 *
 * Exclusive continuum CREATE-ONLY (7721-7730: chdir path length stubs —
 * path_max, path_len_ok, nonempty, name_max, comp_len_ok, is_abs,
 * is_rel, path_room, req_ok, continuum + batch_id_7730). Unique
 * gj_chdir_comp_len_ok_u_7725 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7725_marker[] = "libcgj-batch7725";

/* Soft Linux NAME_MAX for a single path component. */
#define B7725_NAME_MAX  ((uint32_t)255u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7725_comp_len_ok(uint32_t u32Len)
{
	if (u32Len == 0u)
		return 0u;
	if (u32Len > B7725_NAME_MAX)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chdir_comp_len_ok_u_7725 - 1 if path component length is soft-valid.
 *
 * len: soft component length in bytes (no slash, no NUL counted)
 *
 * Returns 1 when 0 < len <= 255, else 0. Pure range test; no parent
 * wires. Does not call chdir.
 */
uint32_t
gj_chdir_comp_len_ok_u_7725(uint32_t u32Len)
{
	(void)NULL;
	return b7725_comp_len_ok(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chdir_comp_len_ok_u_7725(uint32_t u32Len)
    __attribute__((alias("gj_chdir_comp_len_ok_u_7725")));
