/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6942: sendfile NULL offset means current pos.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sendfile_offset_null_means_cur_6942(void);
 *     - Returns 1 (soft tag: a NULL offset pointer uses the current
 *       in_fd file position; non-NULL is updated by the transfer).
 *   uint32_t __gj_sendfile_offset_null_means_cur_6942  (alias)
 *   __libcgj_batch6942_marker = "libcgj-batch6942"
 *
 * Exclusive continuum CREATE-ONLY (6941-6950: sendfile offset stubs).
 * Unique gj_sendfile_offset_null_means_cur_6942 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6942_marker[] = "libcgj-batch6942";

/* Soft POSIX/Linux sendfile contract: NULL offset => use current position. */
#define B6942_NULL_MEANS_CUR  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6942_null_means_cur(void)
{
	return B6942_NULL_MEANS_CUR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sendfile_offset_null_means_cur_6942 - NULL offset uses current pos.
 *
 * Always returns 1. Soft pure-data product tag for the sendfile contract
 * that a null offset pointer reads/writes via the current in_fd seek
 * position. Does not call sendfile. No parent wires.
 */
uint32_t
gj_sendfile_offset_null_means_cur_6942(void)
{
	(void)NULL;
	return b6942_null_means_cur();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sendfile_offset_null_means_cur_6942(void)
    __attribute__((alias("gj_sendfile_offset_null_means_cur_6942")));
