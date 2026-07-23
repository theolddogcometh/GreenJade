/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6930: copy_file_range continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfr_continuum_ok_6930(void);
 *     - Returns 1 (soft lamp: 6921-6930 copy_file_range stubs continuum
 *       complete / ready).
 *   uint32_t gj_batch_id_6930(void);
 *     - Returns the compile-time graph batch number for this TU (6930).
 *   uint32_t __gj_cfr_continuum_ok_6930  (alias)
 *   uint32_t __gj_batch_id_6930  (alias)
 *   __libcgj_batch6930_marker = "libcgj-batch6930"
 *
 * Exclusive continuum CREATE-ONLY (6921-6930: copy_file_range stubs —
 * flags_word_bits_6921, flags_must_zero_6922, flags_ok_6923,
 * off_null_means_cur_6924, fd_ok_6925, len_zero_ok_6926, same_fd_u_6927,
 * nr_x86_64_6928, arg_count/known_flags_6929, continuum + batch_id_6930).
 * Unique surfaces only; no multi-def. Does NOT redefine bare gj_batch_id.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6930_marker[] = "libcgj-batch6930";

#define B6930_CONTINUUM_OK  1u
#define B6930_BATCH_ID      6930u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6930_continuum(void)
{
	return B6930_CONTINUUM_OK;
}

static uint32_t
b6930_id(void)
{
	return B6930_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfr_continuum_ok_6930 - continuum-ready tag for 6921-6930.
 *
 * Always returns 1. Soft pure-data product tag that the copy_file_range
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_cfr_continuum_ok_6930(void)
{
	(void)NULL;
	return b6930_continuum();
}

/*
 * gj_batch_id_6930 - report this TU's graph batch number.
 *
 * Always returns 6930.
 */
uint32_t
gj_batch_id_6930(void)
{
	return b6930_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_cfr_continuum_ok_6930(void)
    __attribute__((alias("gj_cfr_continuum_ok_6930")));

uint32_t __gj_batch_id_6930(void)
    __attribute__((alias("gj_batch_id_6930")));
