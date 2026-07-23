/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8586: locale category LC_MONETARY id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lc_monetary_id_8586(void);
 *     - Return soft LC_MONETARY category code (4).
 *   uint32_t __gj_lc_monetary_id_8586  (alias)
 *   __libcgj_batch8586_marker = "libcgj-batch8586"
 *
 * Exclusive continuum CREATE-ONLY (8581-8590: locale category id stubs —
 * lc_all_id, lc_ctype_id, lc_numeric_id, lc_time_id, lc_collate_id,
 * lc_monetary_id, lc_messages_id, locale_c_ok_u, locale_ready_u,
 * batch_id_8590). Unique gj_lc_monetary_id_8586 surface only; no multi-def.
 * Distinct from gj_locale_cat_monetary_id_7875. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8586_marker[] = "libcgj-batch8586";

/* Soft locale category: LC_MONETARY (currency formats). */
#define B8586_LC_MONETARY  ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8586_monetary_id(void)
{
	return B8586_LC_MONETARY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lc_monetary_id_8586 - soft LC_MONETARY locale category id.
 *
 * Always returns 4. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_lc_monetary_id_8586(void)
{
	(void)NULL;
	return b8586_monetary_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lc_monetary_id_8586(void)
    __attribute__((alias("gj_lc_monetary_id_8586")));
