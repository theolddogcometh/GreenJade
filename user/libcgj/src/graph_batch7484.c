/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7484: mknodat S_IFBLK type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_s_ifblk_id_7484(void);
 *     - Return soft S_IFBLK file-type constant (0x6000 / 0060000).
 *   uint32_t __gj_s_ifblk_id_7484  (alias)
 *   __libcgj_batch7484_marker = "libcgj-batch7484"
 *
 * Exclusive continuum CREATE-ONLY (7481-7490: mknodat type stubs —
 * s_ififo_id, s_ifchr_id, s_ifdir_id, s_ifblk_id, s_ifreg_id,
 * s_iflnk_id, s_ifsock_id, s_ifmt_id, type_ok, batch_id_7490).
 * Unique gj_s_ifblk_id_7484 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7484_marker[] = "libcgj-batch7484";

/* S_IFBLK: block special device type bits (0060000 / 0x6000). */
#define B7484_S_IFBLK ((uint32_t)0x6000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7484_s_ifblk_id(void)
{
	return B7484_S_IFBLK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_s_ifblk_id_7484 - soft S_IFBLK type constant for mknodat.
 *
 * Always returns 0x6000 (Linux/POSIX S_IFBLK). Catalog id only; does
 * not call mknodat. No parent wires.
 */
uint32_t
gj_s_ifblk_id_7484(void)
{
	(void)NULL;
	return b7484_s_ifblk_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_s_ifblk_id_7484(void)
    __attribute__((alias("gj_s_ifblk_id_7484")));
