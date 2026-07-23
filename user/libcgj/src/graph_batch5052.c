/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5052: POSIX S_IFDIR mode bit as uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_mode_dir_u(void);
 *     - Return the directory type mask (octal 0040000 / 0x4000).
 *   uint32_t __gj_fs_mode_dir_u  (alias)
 *   __libcgj_batch5052_marker = "libcgj-batch5052"
 *
 * Exclusive continuum CREATE-ONLY (5051-5060). Distinct from
 * gj_fs_mode_reg_u (batch5051) — unique gj_fs_mode_dir_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5052_marker[] = "libcgj-batch5052";

/* POSIX S_IFDIR: directory type (octal 0040000). */
#define B5052_S_IFDIR  ((uint32_t)0x4000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5052_mode_dir(void)
{
	return B5052_S_IFDIR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_mode_dir_u - return POSIX S_IFDIR (directory) type mask.
 *
 * Always returns 0x4000 (octal 0040000). Freestanding integer constant;
 * no libc stat headers. Self-contained; no parent wires.
 */
uint32_t
gj_fs_mode_dir_u(void)
{
	(void)NULL;
	return b5052_mode_dir();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fs_mode_dir_u(void)
    __attribute__((alias("gj_fs_mode_dir_u")));
