/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7712: getcwd NAME_MAX catalog stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gwd_name_max_7712(void);
 *     - Returns 255 (soft Linux NAME_MAX for path component catalogs).
 *       Pure-data stub; not a live pathconf(3) call.
 *   uint32_t __gj_gwd_name_max_7712  (alias)
 *   __libcgj_batch7712_marker = "libcgj-batch7712"
 *
 * Exclusive continuum CREATE-ONLY (7711-7720: getcwd path max stubs —
 * path_max, name_max, min_buf, buf_ok, root_len, slash, path_len_ok,
 * score, continuum_ready, batch_id_7720). Unique gj_gwd_name_max_7712
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7712_marker[] = "libcgj-batch7712";

/* Soft Linux NAME_MAX for path component catalogs. */
#define B7712_NAME_MAX  ((uint32_t)255u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7712_name_max(void)
{
	return B7712_NAME_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gwd_name_max_7712 - soft NAME_MAX for path components.
 *
 * Always returns 255. Catalog stub for getcwd path-max tables.
 * No parent wires.
 */
uint32_t
gj_gwd_name_max_7712(void)
{
	(void)NULL;
	return b7712_name_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gwd_name_max_7712(void)
    __attribute__((alias("gj_gwd_name_max_7712")));
