/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7880: locale category continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_cat_continuum_ok_7880(void);
 *     - Returns 1 (soft lamp: 7871-7880 locale category stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7880(void);
 *     - Returns the compile-time graph batch number for this TU (7880).
 *   uint32_t __gj_locale_cat_continuum_ok_7880  (alias)
 *   uint32_t __gj_batch_id_7880  (alias)
 *   __libcgj_batch7880_marker = "libcgj-batch7880"
 *
 * Exclusive continuum CREATE-ONLY (7871-7880: locale category stubs —
 * ctype_id, numeric_id, time_id, collate_id, monetary_id, messages_id,
 * all_id, cat_ok, cat_errorish, continuum + batch_id_7880). Unique
 * surfaces only; no multi-def. Does NOT redefine bare gj_batch_id. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7880_marker[] = "libcgj-batch7880";

#define B7880_CONTINUUM_OK  1u
#define B7880_BATCH_ID      7880u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7880_continuum(void)
{
	return B7880_CONTINUUM_OK;
}

static uint32_t
b7880_id(void)
{
	return B7880_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_cat_continuum_ok_7880 - continuum-ready tag for 7871-7880.
 *
 * Always returns 1. Soft pure-data product tag that the locale category
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_locale_cat_continuum_ok_7880(void)
{
	(void)NULL;
	return b7880_continuum();
}

/*
 * gj_batch_id_7880 - report this TU's graph batch number.
 *
 * Always returns 7880.
 */
uint32_t
gj_batch_id_7880(void)
{
	return b7880_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_locale_cat_continuum_ok_7880(void)
    __attribute__((alias("gj_locale_cat_continuum_ok_7880")));

uint32_t __gj_batch_id_7880(void)
    __attribute__((alias("gj_batch_id_7880")));
