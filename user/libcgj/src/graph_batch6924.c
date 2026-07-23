/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6924: copy_file_range NULL offset semantics.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfr_off_null_means_cur_6924(void);
 *     - Returns 1 (soft: NULL off_in / off_out means use current file
 *       position rather than an explicit byte offset).
 *   uint32_t __gj_cfr_off_null_means_cur_6924  (alias)
 *   __libcgj_batch6924_marker = "libcgj-batch6924"
 *
 * Exclusive continuum CREATE-ONLY (6921-6930: copy_file_range stubs).
 * Unique gj_cfr_off_null_means_cur_6924 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6924_marker[] = "libcgj-batch6924";

/* Soft: NULL offset pointers select current file position. */
#define B6924_NULL_MEANS_CUR  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6924_null_means_cur(void)
{
	return B6924_NULL_MEANS_CUR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfr_off_null_means_cur_6924 - soft NULL-offset means current-pos tag.
 *
 * Always returns 1. Pure-data semantic tag for product copy_file_range
 * offset handling; does not touch file descriptors. No parent wires.
 */
uint32_t
gj_cfr_off_null_means_cur_6924(void)
{
	(void)NULL;
	return b6924_null_means_cur();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cfr_off_null_means_cur_6924(void)
    __attribute__((alias("gj_cfr_off_null_means_cur_6924")));
