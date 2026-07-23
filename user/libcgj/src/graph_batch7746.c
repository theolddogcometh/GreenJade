/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7746: linux_dirent64 d_type validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dirent_dt_ok_u_7746(uint32_t d_type);
 *     - Return 1 if d_type is a known soft DT_* code
 *       (UNKNOWN/FIFO/CHR/DIR/BLK/REG/LNK/SOCK/WHT), else 0.
 *   uint32_t __gj_dirent_dt_ok_u_7746  (alias)
 *   __libcgj_batch7746_marker = "libcgj-batch7746"
 *
 * Exclusive continuum CREATE-ONLY (7741-7750: getdents dirent stubs —
 * root, getdents_nr, getdents64_nr, dt_dir, dt_reg, dt_ok, reclen_align,
 * reclen_min, dt_errorish, continuum + batch_id_7750). Unique
 * gj_dirent_dt_ok_u_7746 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7746_marker[] = "libcgj-batch7746";

/* Soft known d_type codes (linux_dirent64). */
#define B7746_DT_UNKNOWN  ((uint32_t)0u)
#define B7746_DT_FIFO     ((uint32_t)1u)
#define B7746_DT_CHR      ((uint32_t)2u)
#define B7746_DT_DIR      ((uint32_t)4u)
#define B7746_DT_BLK      ((uint32_t)6u)
#define B7746_DT_REG      ((uint32_t)8u)
#define B7746_DT_LNK      ((uint32_t)10u)
#define B7746_DT_SOCK     ((uint32_t)12u)
#define B7746_DT_WHT      ((uint32_t)14u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7746_dt_ok(uint32_t u32Type)
{
	if (u32Type == B7746_DT_UNKNOWN) {
		return 1u;
	}
	if (u32Type == B7746_DT_FIFO) {
		return 1u;
	}
	if (u32Type == B7746_DT_CHR) {
		return 1u;
	}
	if (u32Type == B7746_DT_DIR) {
		return 1u;
	}
	if (u32Type == B7746_DT_BLK) {
		return 1u;
	}
	if (u32Type == B7746_DT_REG) {
		return 1u;
	}
	if (u32Type == B7746_DT_LNK) {
		return 1u;
	}
	if (u32Type == B7746_DT_SOCK) {
		return 1u;
	}
	if (u32Type == B7746_DT_WHT) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dirent_dt_ok_u_7746 - 1 if d_type is a known soft DT_* code.
 *
 * d_type: soft linux_dirent64.d_type value
 *
 * Returns 1 for the nine standard DT_* values; else 0. Soft catalog
 * check; does not call getdents. No parent wires.
 */
uint32_t
gj_dirent_dt_ok_u_7746(uint32_t u32Type)
{
	(void)NULL;
	return b7746_dt_ok(u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dirent_dt_ok_u_7746(uint32_t u32Type)
    __attribute__((alias("gj_dirent_dt_ok_u_7746")));
