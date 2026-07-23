/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7481: mknodat S_IFIFO type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_s_ififo_id_7481(void);
 *     - Return soft S_IFIFO file-type constant (0x1000 / 0010000).
 *   uint32_t __gj_s_ififo_id_7481  (alias)
 *   __libcgj_batch7481_marker = "libcgj-batch7481"
 *
 * Exclusive continuum CREATE-ONLY (7481-7490: mknodat type stubs —
 * s_ififo_id, s_ifchr_id, s_ifdir_id, s_ifblk_id, s_ifreg_id,
 * s_iflnk_id, s_ifsock_id, s_ifmt_id, type_ok, batch_id_7490).
 * Unique gj_s_ififo_id_7481 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7481_marker[] = "libcgj-batch7481";

/* S_IFIFO: FIFO special (named pipe) type bits (0010000 / 0x1000). */
#define B7481_S_IFIFO ((uint32_t)0x1000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7481_s_ififo_id(void)
{
	return B7481_S_IFIFO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_s_ififo_id_7481 - soft S_IFIFO type constant for mknodat.
 *
 * Always returns 0x1000 (Linux/POSIX S_IFIFO). Catalog id only; does
 * not call mknodat. No parent wires.
 */
uint32_t
gj_s_ififo_id_7481(void)
{
	(void)NULL;
	return b7481_s_ififo_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_s_ififo_id_7481(void)
    __attribute__((alias("gj_s_ififo_id_7481")));
