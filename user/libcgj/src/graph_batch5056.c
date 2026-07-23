/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5056: test st_mode for symbolic-link type.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_is_lnk_u(uint32_t mode);
 *     - 1 if (mode & S_IFMT) == S_IFLNK, else 0.
 *   uint32_t __gj_fs_is_lnk_u  (alias)
 *   __libcgj_batch5056_marker = "libcgj-batch5056"
 *
 * Exclusive continuum CREATE-ONLY (5051-5060). Distinct from
 * gj_fs_mode_lnk_u (batch5053) / gj_fs_is_reg_u / gj_fs_is_dir_u —
 * unique gj_fs_is_lnk_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5056_marker[] = "libcgj-batch5056";

/* POSIX S_IFMT / S_IFLNK without libc sys/stat.h. */
#define B5056_S_IFMT   ((uint32_t)0xf000u)  /* 0170000 */
#define B5056_S_IFLNK  ((uint32_t)0xa000u)  /* 0120000 */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5056_is_lnk(uint32_t u32Mode)
{
	if ((u32Mode & B5056_S_IFMT) == B5056_S_IFLNK) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_is_lnk_u - 1 if mode encodes a symbolic link, else 0.
 *
 * mode: POSIX-style st_mode bitfield
 *
 * Returns 1 when the type nibble equals S_IFLNK (0xa000), else 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_fs_is_lnk_u(uint32_t u32Mode)
{
	(void)NULL;
	return b5056_is_lnk(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fs_is_lnk_u(uint32_t u32Mode)
    __attribute__((alias("gj_fs_is_lnk_u")));
