/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7483: mknodat S_IFDIR type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_s_ifdir_id_7483(void);
 *     - Return soft S_IFDIR file-type constant (0x4000 / 0040000).
 *   uint32_t __gj_s_ifdir_id_7483  (alias)
 *   __libcgj_batch7483_marker = "libcgj-batch7483"
 *
 * Exclusive continuum CREATE-ONLY (7481-7490: mknodat type stubs —
 * s_ififo_id, s_ifchr_id, s_ifdir_id, s_ifblk_id, s_ifreg_id,
 * s_iflnk_id, s_ifsock_id, s_ifmt_id, type_ok, batch_id_7490).
 * Unique gj_s_ifdir_id_7483 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7483_marker[] = "libcgj-batch7483";

/* S_IFDIR: directory type bits (0040000 / 0x4000). */
#define B7483_S_IFDIR ((uint32_t)0x4000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7483_s_ifdir_id(void)
{
	return B7483_S_IFDIR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_s_ifdir_id_7483 - soft S_IFDIR type constant for mknodat.
 *
 * Always returns 0x4000 (Linux/POSIX S_IFDIR). Catalog id only; does
 * not call mknodat. No parent wires.
 */
uint32_t
gj_s_ifdir_id_7483(void)
{
	(void)NULL;
	return b7483_s_ifdir_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_s_ifdir_id_7483(void)
    __attribute__((alias("gj_s_ifdir_id_7483")));
