/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7488: mknodat S_IFMT type-mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_s_ifmt_id_7488(void);
 *     - Return soft S_IFMT file-type mask constant (0xF000 / 0170000).
 *   uint32_t __gj_s_ifmt_id_7488  (alias)
 *   __libcgj_batch7488_marker = "libcgj-batch7488"
 *
 * Exclusive continuum CREATE-ONLY (7481-7490: mknodat type stubs —
 * s_ififo_id, s_ifchr_id, s_ifdir_id, s_ifblk_id, s_ifreg_id,
 * s_iflnk_id, s_ifsock_id, s_ifmt_id, type_ok, batch_id_7490).
 * Unique gj_s_ifmt_id_7488 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7488_marker[] = "libcgj-batch7488";

/* S_IFMT: file type bit mask (0170000 / 0xF000). */
#define B7488_S_IFMT ((uint32_t)0xF000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7488_s_ifmt_id(void)
{
	return B7488_S_IFMT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_s_ifmt_id_7488 - soft S_IFMT type-mask constant for mknodat.
 *
 * Always returns 0xF000 (Linux/POSIX S_IFMT). Catalog id only; does
 * not call mknodat. No parent wires.
 */
uint32_t
gj_s_ifmt_id_7488(void)
{
	(void)NULL;
	return b7488_s_ifmt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_s_ifmt_id_7488(void)
    __attribute__((alias("gj_s_ifmt_id_7488")));
