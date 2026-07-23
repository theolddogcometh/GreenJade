/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7485: mknodat S_IFREG type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_s_ifreg_id_7485(void);
 *     - Return soft S_IFREG file-type constant (0x8000 / 0100000).
 *   uint32_t __gj_s_ifreg_id_7485  (alias)
 *   __libcgj_batch7485_marker = "libcgj-batch7485"
 *
 * Exclusive continuum CREATE-ONLY (7481-7490: mknodat type stubs —
 * s_ififo_id, s_ifchr_id, s_ifdir_id, s_ifblk_id, s_ifreg_id,
 * s_iflnk_id, s_ifsock_id, s_ifmt_id, type_ok, batch_id_7490).
 * Unique gj_s_ifreg_id_7485 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7485_marker[] = "libcgj-batch7485";

/* S_IFREG: regular file type bits (0100000 / 0x8000). */
#define B7485_S_IFREG ((uint32_t)0x8000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7485_s_ifreg_id(void)
{
	return B7485_S_IFREG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_s_ifreg_id_7485 - soft S_IFREG type constant for mknodat.
 *
 * Always returns 0x8000 (Linux/POSIX S_IFREG). Catalog id only; does
 * not call mknodat. No parent wires.
 */
uint32_t
gj_s_ifreg_id_7485(void)
{
	(void)NULL;
	return b7485_s_ifreg_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_s_ifreg_id_7485(void)
    __attribute__((alias("gj_s_ifreg_id_7485")));
