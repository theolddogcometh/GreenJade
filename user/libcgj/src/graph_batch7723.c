/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7723: chdir nonempty path length stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chdir_path_nonempty_u_7723(uint32_t len);
 *     - Return 1 if len > 0 (non-empty path), else 0.
 *   uint32_t __gj_chdir_path_nonempty_u_7723  (alias)
 *   __libcgj_batch7723_marker = "libcgj-batch7723"
 *
 * Exclusive continuum CREATE-ONLY (7721-7730: chdir path length stubs —
 * path_max, path_len_ok, nonempty, name_max, comp_len_ok, is_abs,
 * is_rel, path_room, req_ok, continuum + batch_id_7730). Unique
 * gj_chdir_path_nonempty_u_7723 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7723_marker[] = "libcgj-batch7723";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7723_path_nonempty(uint32_t u32Len)
{
	return (u32Len > 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chdir_path_nonempty_u_7723 - 1 if path length is non-zero.
 *
 * len: soft path length in bytes
 *
 * Returns 1 when len > 0, else 0. Empty paths are rejected by chdir
 * soft policy. Pure length test; no parent wires. Does not call chdir.
 */
uint32_t
gj_chdir_path_nonempty_u_7723(uint32_t u32Len)
{
	(void)NULL;
	return b7723_path_nonempty(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chdir_path_nonempty_u_7723(uint32_t u32Len)
    __attribute__((alias("gj_chdir_path_nonempty_u_7723")));
