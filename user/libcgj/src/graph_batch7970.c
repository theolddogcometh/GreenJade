/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7970: bsearch key continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bsearch_key_continuum_ok_7970(void);
 *     - Returns 1 (soft lamp: 7961-7970 bsearch key stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7970(void);
 *     - Returns the compile-time graph batch number for this TU (7970).
 *   uint32_t __gj_bsearch_key_continuum_ok_7970  (alias)
 *   uint32_t __gj_batch_id_7970  (alias)
 *   __libcgj_batch7970_marker = "libcgj-batch7970"
 *
 * Exclusive continuum CREATE-ONLY (7961-7970: bsearch key stubs —
 * key_present_ok, base_present_ok, nmemb_zero_ok, width_ok,
 * cmp_present_ok, cmp_eq_id, cmp_lt_id, cmp_gt_id, params_ok,
 * continuum + batch_id_7970). Unique surfaces only; no multi-def.
 * Does NOT redefine bare gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7970_marker[] = "libcgj-batch7970";

#define B7970_CONTINUUM_OK  1u
#define B7970_BATCH_ID      7970u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7970_continuum(void)
{
	return B7970_CONTINUUM_OK;
}

static uint32_t
b7970_id(void)
{
	return B7970_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_key_continuum_ok_7970 - continuum-ready tag for 7961-7970.
 *
 * Always returns 1. Soft pure-data product tag that the bsearch key
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_bsearch_key_continuum_ok_7970(void)
{
	(void)NULL;
	return b7970_continuum();
}

/*
 * gj_batch_id_7970 - report this TU's graph batch number.
 *
 * Always returns 7970.
 */
uint32_t
gj_batch_id_7970(void)
{
	return b7970_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_bsearch_key_continuum_ok_7970(void)
    __attribute__((alias("gj_bsearch_key_continuum_ok_7970")));

uint32_t __gj_batch_id_7970(void)
    __attribute__((alias("gj_batch_id_7970")));
