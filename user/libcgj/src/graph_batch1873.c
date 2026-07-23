/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1873: Unix mode symbolic-link type hint.
 *
 * Surface (unique symbols):
 *   int gj_mode_is_lnk_hint(uint32_t mode);
 *     — Return 1 if mode_t-like u32 has file-type bits equal to S_IFLNK
 *       (symbolic link). Compares (mode & S_IFMT) == S_IFLNK using the
 *       standard Unix numeric constants (no sys/stat.h):
 *         S_IFMT  = 0170000 (0xF000)  file type mask
 *         S_IFLNK = 0120000 (0xA000)  symbolic link
 *       Lightweight pure-data hint only.
 *   int __gj_mode_is_lnk_hint  (alias)
 *   __libcgj_batch1873_marker = "libcgj-batch1873"
 *
 * Filesystem exclusive pure-data wave (1871–1879); mode bit predicates.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1873_marker[] = "libcgj-batch1873";

/* Unix st_mode file-type mask / symlink value (octal). */
#define B1873_IFMT   0170000u
#define B1873_IFLNK  0120000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1873_is_lnk(uint32_t uMode)
{
	return ((uMode & B1873_IFMT) == B1873_IFLNK) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_is_lnk_hint — 1 if mode has S_IFLNK file-type bits.
 *
 * mode: mode_t-like uint32_t (st_mode / stx_mode style)
 * Returns 1 when (mode & 0170000) == 0120000, else 0.
 */
int
gj_mode_is_lnk_hint(uint32_t uMode)
{
	return b1873_is_lnk(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mode_is_lnk_hint(uint32_t uMode)
    __attribute__((alias("gj_mode_is_lnk_hint")));
