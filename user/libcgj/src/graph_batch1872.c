/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1872: Unix mode directory type hint.
 *
 * Surface (unique symbols):
 *   int gj_mode_is_dir_hint(uint32_t mode);
 *     — Return 1 if mode_t-like u32 has file-type bits equal to S_IFDIR
 *       (directory). Compares (mode & S_IFMT) == S_IFDIR using the
 *       standard Unix numeric constants (no sys/stat.h):
 *         S_IFMT  = 0170000 (0xF000)  file type mask
 *         S_IFDIR = 0040000 (0x4000)  directory
 *       Lightweight pure-data hint only.
 *   int __gj_mode_is_dir_hint  (alias)
 *   __libcgj_batch1872_marker = "libcgj-batch1872"
 *
 * Filesystem exclusive pure-data wave (1871–1879); mode bit predicates.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1872_marker[] = "libcgj-batch1872";

/* Unix st_mode file-type mask / directory value (octal). */
#define B1872_IFMT   0170000u
#define B1872_IFDIR  0040000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1872_is_dir(uint32_t uMode)
{
	return ((uMode & B1872_IFMT) == B1872_IFDIR) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_is_dir_hint — 1 if mode has S_IFDIR file-type bits.
 *
 * mode: mode_t-like uint32_t (st_mode / stx_mode style)
 * Returns 1 when (mode & 0170000) == 0040000, else 0.
 */
int
gj_mode_is_dir_hint(uint32_t uMode)
{
	return b1872_is_dir(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mode_is_dir_hint(uint32_t uMode)
    __attribute__((alias("gj_mode_is_dir_hint")));
