/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6885: statx mask timestamp bits soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_mask_atime_6885(void);
 *     - Returns soft STATX_ATIME bit (0x00000020).
 *   uint32_t gj_statx_mask_mtime_6885(void);
 *     - Returns soft STATX_MTIME bit (0x00000040).
 *   uint32_t gj_statx_mask_ctime_6885(void);
 *     - Returns soft STATX_CTIME bit (0x00000080).
 *   uint32_t gj_statx_mask_times_6885(void);
 *     - Returns soft STATX_ATIME|MTIME|CTIME (0x000000E0).
 *   uint32_t __gj_statx_mask_atime_6885  (alias)
 *   uint32_t __gj_statx_mask_mtime_6885  (alias)
 *   uint32_t __gj_statx_mask_ctime_6885  (alias)
 *   uint32_t __gj_statx_mask_times_6885  (alias)
 *   __libcgj_batch6885_marker = "libcgj-batch6885"
 *
 * Exclusive continuum CREATE-ONLY (6881-6890: statx mask stubs).
 * Unique gj_statx_mask_*_6885 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6885_marker[] = "libcgj-batch6885";

/* Soft Linux STATX_{A,M,C}TIME (stx_*time present). BTIME is batch6887. */
#define B6885_STATX_ATIME  0x00000020u
#define B6885_STATX_MTIME  0x00000040u
#define B6885_STATX_CTIME  0x00000080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6885_atime(void)
{
	return B6885_STATX_ATIME;
}

static uint32_t
b6885_mtime(void)
{
	return B6885_STATX_MTIME;
}

static uint32_t
b6885_ctime(void)
{
	return B6885_STATX_CTIME;
}

static uint32_t
b6885_times(void)
{
	return B6885_STATX_ATIME | B6885_STATX_MTIME | B6885_STATX_CTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_mask_atime_6885 - soft STATX_ATIME request bit.
 *
 * Always returns 0x00000020. Soft pure-data product tag; no statx.
 */
uint32_t
gj_statx_mask_atime_6885(void)
{
	(void)NULL;
	return b6885_atime();
}

/*
 * gj_statx_mask_mtime_6885 - soft STATX_MTIME request bit.
 *
 * Always returns 0x00000040.
 */
uint32_t
gj_statx_mask_mtime_6885(void)
{
	return b6885_mtime();
}

/*
 * gj_statx_mask_ctime_6885 - soft STATX_CTIME request bit.
 *
 * Always returns 0x00000080.
 */
uint32_t
gj_statx_mask_ctime_6885(void)
{
	return b6885_ctime();
}

/*
 * gj_statx_mask_times_6885 - soft STATX_ATIME|MTIME|CTIME merge.
 *
 * Always returns 0x000000E0. Basic timestamp trio (not BTIME).
 */
uint32_t
gj_statx_mask_times_6885(void)
{
	return b6885_times();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_statx_mask_atime_6885(void)
    __attribute__((alias("gj_statx_mask_atime_6885")));

uint32_t __gj_statx_mask_mtime_6885(void)
    __attribute__((alias("gj_statx_mask_mtime_6885")));

uint32_t __gj_statx_mask_ctime_6885(void)
    __attribute__((alias("gj_statx_mask_ctime_6885")));

uint32_t __gj_statx_mask_times_6885(void)
    __attribute__((alias("gj_statx_mask_times_6885")));
