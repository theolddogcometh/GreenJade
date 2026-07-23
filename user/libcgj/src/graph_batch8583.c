/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8583: locale category LC_NUMERIC id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lc_numeric_id_8583(void);
 *     - Return soft LC_NUMERIC category code (1).
 *   uint32_t __gj_lc_numeric_id_8583  (alias)
 *   __libcgj_batch8583_marker = "libcgj-batch8583"
 *
 * Exclusive continuum CREATE-ONLY (8581-8590: locale category id stubs —
 * lc_all_id, lc_ctype_id, lc_numeric_id, lc_time_id, lc_collate_id,
 * lc_monetary_id, lc_messages_id, locale_c_ok_u, locale_ready_u,
 * batch_id_8590). Unique gj_lc_numeric_id_8583 surface only; no multi-def.
 * Distinct from gj_locale_cat_numeric_id_7872. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8583_marker[] = "libcgj-batch8583";

/* Soft locale category: LC_NUMERIC (decimal point / grouping). */
#define B8583_LC_NUMERIC  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8583_numeric_id(void)
{
	return B8583_LC_NUMERIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lc_numeric_id_8583 - soft LC_NUMERIC locale category id.
 *
 * Always returns 1. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_lc_numeric_id_8583(void)
{
	(void)NULL;
	return b8583_numeric_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lc_numeric_id_8583(void)
    __attribute__((alias("gj_lc_numeric_id_8583")));
