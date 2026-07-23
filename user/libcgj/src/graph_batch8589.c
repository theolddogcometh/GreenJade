/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8589: locale continuum ready lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_ready_u_8589(void);
 *     - Return 1 (soft lamp: 8581-8590 locale category id stubs
 *       continuum ready).
 *   uint32_t __gj_locale_ready_u_8589  (alias)
 *   __libcgj_batch8589_marker = "libcgj-batch8589"
 *
 * Exclusive continuum CREATE-ONLY (8581-8590: locale category id stubs —
 * lc_all_id, lc_ctype_id, lc_numeric_id, lc_time_id, lc_collate_id,
 * lc_monetary_id, lc_messages_id, locale_c_ok_u, locale_ready_u,
 * batch_id_8590). Unique gj_locale_ready_u_8589 surface only; no multi-def.
 * Distinct from gj_locale_cat_continuum_ok_7880. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8589_marker[] = "libcgj-batch8589";

/* Soft continuum-ready lamp for locale category id stubs wave. */
#define B8589_LOCALE_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8589_ready(void)
{
	return B8589_LOCALE_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_ready_u_8589 - soft locale continuum ready lamp.
 *
 * Always returns 1. Pure-data product tag that the 8581-8590 locale
 * category id stubs exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_locale_ready_u_8589(void)
{
	(void)NULL;
	return b8589_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_ready_u_8589(void)
    __attribute__((alias("gj_locale_ready_u_8589")));
