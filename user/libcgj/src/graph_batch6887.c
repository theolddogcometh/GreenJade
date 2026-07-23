/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6887: statx mask STATX_BASIC_STATS soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_mask_basic_stats_6887(void);
 *     - Returns soft STATX_BASIC_STATS (0x000007FF).
 *   uint32_t gj_statx_mask_btime_6887(void);
 *     - Returns soft STATX_BTIME bit (0x00000800).
 *   uint32_t gj_statx_mask_mnt_id_6887(void);
 *     - Returns soft STATX_MNT_ID bit (0x00001000).
 *   uint32_t __gj_statx_mask_basic_stats_6887  (alias)
 *   uint32_t __gj_statx_mask_btime_6887  (alias)
 *   uint32_t __gj_statx_mask_mnt_id_6887  (alias)
 *   __libcgj_batch6887_marker = "libcgj-batch6887"
 *
 * Exclusive continuum CREATE-ONLY (6881-6890: statx mask stubs).
 * Unique gj_statx_mask_*_6887 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6887_marker[] = "libcgj-batch6887";

/*
 * Soft Linux:
 *   STATX_BASIC_STATS = TYPE|MODE|NLINK|UID|GID|ATIME|MTIME|CTIME|
 *                       INO|SIZE|BLOCKS = 0x000007FF
 *   STATX_BTIME  = 0x00000800
 *   STATX_MNT_ID = 0x00001000
 */
#define B6887_STATX_BASIC_STATS  0x000007FFu
#define B6887_STATX_BTIME        0x00000800u
#define B6887_STATX_MNT_ID       0x00001000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6887_basic(void)
{
	return B6887_STATX_BASIC_STATS;
}

static uint32_t
b6887_btime(void)
{
	return B6887_STATX_BTIME;
}

static uint32_t
b6887_mnt_id(void)
{
	return B6887_STATX_MNT_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_mask_basic_stats_6887 - soft STATX_BASIC_STATS baseline.
 *
 * Always returns 0x000007FF. Soft pure-data product tag matching the
 * common "stat-like" field set. Does not call the statx syscall.
 * No parent wires.
 */
uint32_t
gj_statx_mask_basic_stats_6887(void)
{
	(void)NULL;
	return b6887_basic();
}

/*
 * gj_statx_mask_btime_6887 - soft STATX_BTIME request bit.
 *
 * Always returns 0x00000800 (birth / creation time field).
 */
uint32_t
gj_statx_mask_btime_6887(void)
{
	return b6887_btime();
}

/*
 * gj_statx_mask_mnt_id_6887 - soft STATX_MNT_ID request bit.
 *
 * Always returns 0x00001000 (stx_mnt_id present).
 */
uint32_t
gj_statx_mask_mnt_id_6887(void)
{
	return b6887_mnt_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_statx_mask_basic_stats_6887(void)
    __attribute__((alias("gj_statx_mask_basic_stats_6887")));

uint32_t __gj_statx_mask_btime_6887(void)
    __attribute__((alias("gj_statx_mask_btime_6887")));

uint32_t __gj_statx_mask_mnt_id_6887(void)
    __attribute__((alias("gj_statx_mask_mnt_id_6887")));
