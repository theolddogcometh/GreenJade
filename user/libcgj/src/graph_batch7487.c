/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7487: mknodat S_IFSOCK type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_s_ifsock_id_7487(void);
 *     - Return soft S_IFSOCK file-type constant (0xC000 / 0140000).
 *   uint32_t __gj_s_ifsock_id_7487  (alias)
 *   __libcgj_batch7487_marker = "libcgj-batch7487"
 *
 * Exclusive continuum CREATE-ONLY (7481-7490: mknodat type stubs —
 * s_ififo_id, s_ifchr_id, s_ifdir_id, s_ifblk_id, s_ifreg_id,
 * s_iflnk_id, s_ifsock_id, s_ifmt_id, type_ok, batch_id_7490).
 * Unique gj_s_ifsock_id_7487 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7487_marker[] = "libcgj-batch7487";

/* S_IFSOCK: socket type bits (0140000 / 0xC000). */
#define B7487_S_IFSOCK ((uint32_t)0xC000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7487_s_ifsock_id(void)
{
	return B7487_S_IFSOCK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_s_ifsock_id_7487 - soft S_IFSOCK type constant for mknodat.
 *
 * Always returns 0xC000 (Linux/POSIX S_IFSOCK). Catalog id only; does
 * not call mknodat. No parent wires.
 */
uint32_t
gj_s_ifsock_id_7487(void)
{
	(void)NULL;
	return b7487_s_ifsock_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_s_ifsock_id_7487(void)
    __attribute__((alias("gj_s_ifsock_id_7487")));
