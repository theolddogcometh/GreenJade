/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7744: linux_dirent64 DT_DIR type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dirent_dt_dir_id_7744(void);
 *     - Return soft DT_DIR d_type constant (4).
 *   uint32_t __gj_dirent_dt_dir_id_7744  (alias)
 *   __libcgj_batch7744_marker = "libcgj-batch7744"
 *
 * Exclusive continuum CREATE-ONLY (7741-7750: getdents dirent stubs —
 * root, getdents_nr, getdents64_nr, dt_dir, dt_reg, dt_ok, reclen_align,
 * reclen_min, dt_errorish, continuum + batch_id_7750). Unique
 * gj_dirent_dt_dir_id_7744 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7744_marker[] = "libcgj-batch7744";

/* Soft DT_DIR (directory) for linux_dirent64.d_type. */
#define B7744_DT_DIR  ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7744_dt_dir(void)
{
	return B7744_DT_DIR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dirent_dt_dir_id_7744 - soft DT_DIR d_type constant.
 *
 * Always returns 4. Catalog id only; does not call getdents or readdir.
 * No parent wires.
 */
uint32_t
gj_dirent_dt_dir_id_7744(void)
{
	(void)NULL;
	return b7744_dt_dir();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dirent_dt_dir_id_7744(void)
    __attribute__((alias("gj_dirent_dt_dir_id_7744")));
