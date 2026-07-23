/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7714: getcwd buffer size soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gwd_buf_ok_u_7714(uint32_t buf_size);
 *     - Return 1 if buf_size is in [2, 4096] (min useful getcwd buf
 *       through soft PATH_MAX), else 0. Pure-data bound check.
 *   uint32_t __gj_gwd_buf_ok_u_7714  (alias)
 *   __libcgj_batch7714_marker = "libcgj-batch7714"
 *
 * Exclusive continuum CREATE-ONLY (7711-7720: getcwd path max stubs —
 * path_max, name_max, min_buf, buf_ok, root_len, slash, path_len_ok,
 * score, continuum_ready, batch_id_7720). Unique gj_gwd_buf_ok_u_7714
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7714_marker[] = "libcgj-batch7714";

#define B7714_MIN_BUF   ((uint32_t)2u)
#define B7714_PATH_MAX  ((uint32_t)4096u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7714_buf_ok(uint32_t u32Buf)
{
	return (u32Buf >= B7714_MIN_BUF && u32Buf <= B7714_PATH_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gwd_buf_ok_u_7714 - 1 if getcwd buffer size is soft-ok.
 *
 * buf_size: proposed getcwd output buffer length in bytes.
 * Soft bounds only; does not call getcwd. No parent wires.
 */
uint32_t
gj_gwd_buf_ok_u_7714(uint32_t u32BufSize)
{
	(void)NULL;
	return b7714_buf_ok(u32BufSize);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gwd_buf_ok_u_7714(uint32_t u32BufSize)
    __attribute__((alias("gj_gwd_buf_ok_u_7714")));
