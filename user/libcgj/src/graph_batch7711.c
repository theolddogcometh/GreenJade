/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7711: getcwd PATH_MAX catalog stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gwd_path_max_7711(void);
 *     - Returns 4096 (soft Linux PATH_MAX for getcwd buffer catalogs).
 *       Pure-data stub; not a live getcwd(3) call.
 *   uint32_t __gj_gwd_path_max_7711  (alias)
 *   __libcgj_batch7711_marker = "libcgj-batch7711"
 *
 * Exclusive continuum CREATE-ONLY (7711-7720: getcwd path max stubs —
 * path_max, name_max, min_buf, buf_ok, root_len, slash, path_len_ok,
 * score, continuum_ready, batch_id_7720). Unique gj_gwd_path_max_7711
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7711_marker[] = "libcgj-batch7711";

/* Soft Linux PATH_MAX for getcwd buffer catalogs. */
#define B7711_PATH_MAX  ((uint32_t)4096u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7711_path_max(void)
{
	return B7711_PATH_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gwd_path_max_7711 - soft PATH_MAX for getcwd catalogs.
 *
 * Always returns 4096. Catalog stub for getcwd path-max tables.
 * No parent wires.
 */
uint32_t
gj_gwd_path_max_7711(void)
{
	(void)NULL;
	return b7711_path_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gwd_path_max_7711(void)
    __attribute__((alias("gj_gwd_path_max_7711")));
