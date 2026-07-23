/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7745: linux_dirent64 DT_REG type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dirent_dt_reg_id_7745(void);
 *     - Return soft DT_REG d_type constant (8).
 *   uint32_t __gj_dirent_dt_reg_id_7745  (alias)
 *   __libcgj_batch7745_marker = "libcgj-batch7745"
 *
 * Exclusive continuum CREATE-ONLY (7741-7750: getdents dirent stubs —
 * root, getdents_nr, getdents64_nr, dt_dir, dt_reg, dt_ok, reclen_align,
 * reclen_min, dt_errorish, continuum + batch_id_7750). Unique
 * gj_dirent_dt_reg_id_7745 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7745_marker[] = "libcgj-batch7745";

/* Soft DT_REG (regular file) for linux_dirent64.d_type. */
#define B7745_DT_REG  ((uint32_t)8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7745_dt_reg(void)
{
	return B7745_DT_REG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dirent_dt_reg_id_7745 - soft DT_REG d_type constant.
 *
 * Always returns 8. Catalog id only; does not call getdents or readdir.
 * No parent wires.
 */
uint32_t
gj_dirent_dt_reg_id_7745(void)
{
	(void)NULL;
	return b7745_dt_reg();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dirent_dt_reg_id_7745(void)
    __attribute__((alias("gj_dirent_dt_reg_id_7745")));
