/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7486: mknodat S_IFLNK type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_s_iflnk_id_7486(void);
 *     - Return soft S_IFLNK file-type constant (0xA000 / 0120000).
 *   uint32_t __gj_s_iflnk_id_7486  (alias)
 *   __libcgj_batch7486_marker = "libcgj-batch7486"
 *
 * Exclusive continuum CREATE-ONLY (7481-7490: mknodat type stubs —
 * s_ififo_id, s_ifchr_id, s_ifdir_id, s_ifblk_id, s_ifreg_id,
 * s_iflnk_id, s_ifsock_id, s_ifmt_id, type_ok, batch_id_7490).
 * Unique gj_s_iflnk_id_7486 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7486_marker[] = "libcgj-batch7486";

/* S_IFLNK: symbolic link type bits (0120000 / 0xA000). */
#define B7486_S_IFLNK ((uint32_t)0xA000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7486_s_iflnk_id(void)
{
	return B7486_S_IFLNK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_s_iflnk_id_7486 - soft S_IFLNK type constant for mknodat.
 *
 * Always returns 0xA000 (Linux/POSIX S_IFLNK). Catalog id only; does
 * not call mknodat. No parent wires.
 */
uint32_t
gj_s_iflnk_id_7486(void)
{
	(void)NULL;
	return b7486_s_iflnk_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_s_iflnk_id_7486(void)
    __attribute__((alias("gj_s_iflnk_id_7486")));
