/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1877: Unix mode block-device type hint.
 *
 * Surface (unique symbols):
 *   int gj_mode_is_blk_hint(uint32_t mode);
 *     — Return 1 if mode_t-like u32 has file-type bits equal to S_IFBLK
 *       (block device). Compares (mode & S_IFMT) == S_IFBLK using the
 *       standard Unix numeric constants (no sys/stat.h):
 *         S_IFMT  = 0170000 (0xF000)  file type mask
 *         S_IFBLK = 0060000 (0x6000)  block special
 *       Lightweight pure-data hint only.
 *   int __gj_mode_is_blk_hint  (alias)
 *   __libcgj_batch1877_marker = "libcgj-batch1877"
 *
 * Filesystem exclusive pure-data wave (1871–1879); mode bit predicates.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1877_marker[] = "libcgj-batch1877";

/* Unix st_mode file-type mask / block-device value (octal). */
#define B1877_IFMT   0170000u
#define B1877_IFBLK  0060000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1877_is_blk(uint32_t uMode)
{
	return ((uMode & B1877_IFMT) == B1877_IFBLK) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_is_blk_hint — 1 if mode has S_IFBLK file-type bits.
 *
 * mode: mode_t-like uint32_t (st_mode / stx_mode style)
 * Returns 1 when (mode & 0170000) == 0060000, else 0.
 */
int
gj_mode_is_blk_hint(uint32_t uMode)
{
	return b1877_is_blk(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mode_is_blk_hint(uint32_t uMode)
    __attribute__((alias("gj_mode_is_blk_hint")));
