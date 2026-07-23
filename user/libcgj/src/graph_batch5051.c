/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5051: POSIX S_IFREG mode bit as uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_mode_reg_u(void);
 *     - Return the regular-file type mask (octal 0100000 / 0x8000).
 *   uint32_t __gj_fs_mode_reg_u  (alias)
 *   __libcgj_batch5051_marker = "libcgj-batch5051"
 *
 * Exclusive continuum CREATE-ONLY (5051-5060: fs_mode_reg_u, fs_mode_dir_u,
 * fs_mode_lnk_u, fs_is_reg_u, fs_is_dir_u, fs_is_lnk_u, fs_perm_u,
 * fs_type_u, fs_sticky_u, batch_id_5060). Unique gj_fs_mode_reg_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5051_marker[] = "libcgj-batch5051";

/* POSIX S_IFREG: regular file type (octal 0100000). */
#define B5051_S_IFREG  ((uint32_t)0x8000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5051_mode_reg(void)
{
	return B5051_S_IFREG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_mode_reg_u - return POSIX S_IFREG (regular file) type mask.
 *
 * Always returns 0x8000 (octal 0100000). Freestanding integer constant;
 * no libc stat headers. Self-contained; no parent wires.
 */
uint32_t
gj_fs_mode_reg_u(void)
{
	(void)NULL;
	return b5051_mode_reg();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fs_mode_reg_u(void)
    __attribute__((alias("gj_fs_mode_reg_u")));
