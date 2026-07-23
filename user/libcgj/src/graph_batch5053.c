/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5053: POSIX S_IFLNK mode bit as uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_mode_lnk_u(void);
 *     - Return the symbolic-link type mask (octal 0120000 / 0xa000).
 *   uint32_t __gj_fs_mode_lnk_u  (alias)
 *   __libcgj_batch5053_marker = "libcgj-batch5053"
 *
 * Exclusive continuum CREATE-ONLY (5051-5060). Distinct from
 * gj_fs_mode_reg_u / gj_fs_mode_dir_u — unique gj_fs_mode_lnk_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5053_marker[] = "libcgj-batch5053";

/* POSIX S_IFLNK: symbolic link type (octal 0120000). */
#define B5053_S_IFLNK  ((uint32_t)0xa000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5053_mode_lnk(void)
{
	return B5053_S_IFLNK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_mode_lnk_u - return POSIX S_IFLNK (symlink) type mask.
 *
 * Always returns 0xa000 (octal 0120000). Freestanding integer constant;
 * no libc stat headers. Self-contained; no parent wires.
 */
uint32_t
gj_fs_mode_lnk_u(void)
{
	(void)NULL;
	return b5053_mode_lnk();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fs_mode_lnk_u(void)
    __attribute__((alias("gj_fs_mode_lnk_u")));
