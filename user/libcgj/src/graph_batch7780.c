/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7780: scandir filter continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scandir_filter_continuum_ok_7780(void);
 *     - Returns 1 (soft lamp: 7771-7780 scandir filter stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7780(void);
 *     - Returns the compile-time graph batch number for this TU (7780).
 *   uint32_t __gj_scandir_filter_continuum_ok_7780  (alias)
 *   uint32_t __gj_batch_id_7780  (alias)
 *   __libcgj_batch7780_marker = "libcgj-batch7780"
 *
 * Exclusive continuum CREATE-ONLY (7771-7780: scandir filter stubs —
 * accept_all_id, reject_dot_id, reject_hidden_id, is_accept_all,
 * is_reject_dot, is_reject_hidden, filter_ok, filter_pack,
 * filter_errorish, continuum + batch_id_7780). Unique surfaces only; no
 * multi-def. Does NOT redefine bare gj_batch_id. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7780_marker[] = "libcgj-batch7780";

#define B7780_CONTINUUM_OK  1u
#define B7780_BATCH_ID      7780u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7780_continuum(void)
{
	return B7780_CONTINUUM_OK;
}

static uint32_t
b7780_id(void)
{
	return B7780_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scandir_filter_continuum_ok_7780 - continuum-ready tag for 7771-7780.
 *
 * Always returns 1. Soft pure-data product tag that the scandir filter
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_scandir_filter_continuum_ok_7780(void)
{
	(void)NULL;
	return b7780_continuum();
}

/*
 * gj_batch_id_7780 - report this TU's graph batch number.
 *
 * Always returns 7780.
 */
uint32_t
gj_batch_id_7780(void)
{
	return b7780_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_scandir_filter_continuum_ok_7780(void)
    __attribute__((alias("gj_scandir_filter_continuum_ok_7780")));

uint32_t __gj_batch_id_7780(void)
    __attribute__((alias("gj_batch_id_7780")));
