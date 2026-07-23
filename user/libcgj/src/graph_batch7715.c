/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7715: getcwd root path length stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gwd_root_len_7715(void);
 *     - Returns 1 (byte length of root path "/"). Pure-data stub; not a
 *       live getcwd(3) call.
 *   uint32_t __gj_gwd_root_len_7715  (alias)
 *   __libcgj_batch7715_marker = "libcgj-batch7715"
 *
 * Exclusive continuum CREATE-ONLY (7711-7720: getcwd path max stubs —
 * path_max, name_max, min_buf, buf_ok, root_len, slash, path_len_ok,
 * score, continuum_ready, batch_id_7720). Unique gj_gwd_root_len_7715
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7715_marker[] = "libcgj-batch7715";

/* Byte length of root path "/" (excluding NUL). */
#define B7715_ROOT_LEN  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7715_root_len(void)
{
	return B7715_ROOT_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gwd_root_len_7715 - soft root path "/" length.
 *
 * Always returns 1. Catalog stub for getcwd path-max tables.
 * No parent wires.
 */
uint32_t
gj_gwd_root_len_7715(void)
{
	(void)NULL;
	return b7715_root_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gwd_root_len_7715(void)
    __attribute__((alias("gj_gwd_root_len_7715")));
