/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7749: linux_dirent64 d_type errorish stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dirent_dt_errorish_u_7749(uint32_t d_type);
 *     - Return 1 if d_type is not a known soft DT_* code, else 0.
 *   uint32_t __gj_dirent_dt_errorish_u_7749  (alias)
 *   __libcgj_batch7749_marker = "libcgj-batch7749"
 *
 * Exclusive continuum CREATE-ONLY (7741-7750: getdents dirent stubs —
 * root, getdents_nr, getdents64_nr, dt_dir, dt_reg, dt_ok, reclen_align,
 * reclen_min, dt_errorish, continuum + batch_id_7750). Unique
 * gj_dirent_dt_errorish_u_7749 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7749_marker[] = "libcgj-batch7749";

/* Soft known d_type codes (linux_dirent64). */
#define B7749_DT_UNKNOWN  ((uint32_t)0u)
#define B7749_DT_FIFO     ((uint32_t)1u)
#define B7749_DT_CHR      ((uint32_t)2u)
#define B7749_DT_DIR      ((uint32_t)4u)
#define B7749_DT_BLK      ((uint32_t)6u)
#define B7749_DT_REG      ((uint32_t)8u)
#define B7749_DT_LNK      ((uint32_t)10u)
#define B7749_DT_SOCK     ((uint32_t)12u)
#define B7749_DT_WHT      ((uint32_t)14u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7749_dt_errorish(uint32_t u32Type)
{
	if (u32Type == B7749_DT_UNKNOWN) {
		return 0u;
	}
	if (u32Type == B7749_DT_FIFO) {
		return 0u;
	}
	if (u32Type == B7749_DT_CHR) {
		return 0u;
	}
	if (u32Type == B7749_DT_DIR) {
		return 0u;
	}
	if (u32Type == B7749_DT_BLK) {
		return 0u;
	}
	if (u32Type == B7749_DT_REG) {
		return 0u;
	}
	if (u32Type == B7749_DT_LNK) {
		return 0u;
	}
	if (u32Type == B7749_DT_SOCK) {
		return 0u;
	}
	if (u32Type == B7749_DT_WHT) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dirent_dt_errorish_u_7749 - 1 if d_type is unknown/errorish.
 *
 * d_type: soft linux_dirent64.d_type value
 *
 * Returns 1 when d_type is not a standard DT_* code; else 0. Soft
 * inverse of dt_ok; does not call getdents. No parent wires.
 */
uint32_t
gj_dirent_dt_errorish_u_7749(uint32_t u32Type)
{
	(void)NULL;
	return b7749_dt_errorish(u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dirent_dt_errorish_u_7749(uint32_t u32Type)
    __attribute__((alias("gj_dirent_dt_errorish_u_7749")));
