/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7717: getcwd path length soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gwd_path_len_ok_u_7717(uint32_t path_len);
 *     - Return 1 if path_len is in [1, 4096] (non-empty path within soft
 *       PATH_MAX), else 0. Pure-data bound check.
 *   uint32_t __gj_gwd_path_len_ok_u_7717  (alias)
 *   __libcgj_batch7717_marker = "libcgj-batch7717"
 *
 * Exclusive continuum CREATE-ONLY (7711-7720: getcwd path max stubs —
 * path_max, name_max, min_buf, buf_ok, root_len, slash, path_len_ok,
 * score, continuum_ready, batch_id_7720). Unique
 * gj_gwd_path_len_ok_u_7717 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7717_marker[] = "libcgj-batch7717";

#define B7717_PATH_MAX  ((uint32_t)4096u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7717_path_len_ok(uint32_t u32Len)
{
	return (u32Len > 0u && u32Len <= B7717_PATH_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gwd_path_len_ok_u_7717 - 1 if cwd path length is soft-ok.
 *
 * path_len: byte length of a cwd path string (excluding or including
 * NUL is caller-defined; this stub only checks soft bounds).
 * Soft bounds only; does not call getcwd. No parent wires.
 */
uint32_t
gj_gwd_path_len_ok_u_7717(uint32_t u32PathLen)
{
	(void)NULL;
	return b7717_path_len_ok(u32PathLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gwd_path_len_ok_u_7717(uint32_t u32PathLen)
    __attribute__((alias("gj_gwd_path_len_ok_u_7717")));
