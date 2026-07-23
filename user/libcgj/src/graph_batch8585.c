/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8585: locale category LC_COLLATE id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lc_collate_id_8585(void);
 *     - Return soft LC_COLLATE category code (3).
 *   uint32_t __gj_lc_collate_id_8585  (alias)
 *   __libcgj_batch8585_marker = "libcgj-batch8585"
 *
 * Exclusive continuum CREATE-ONLY (8581-8590: locale category id stubs —
 * lc_all_id, lc_ctype_id, lc_numeric_id, lc_time_id, lc_collate_id,
 * lc_monetary_id, lc_messages_id, locale_c_ok_u, locale_ready_u,
 * batch_id_8590). Unique gj_lc_collate_id_8585 surface only; no multi-def.
 * Distinct from gj_locale_cat_collate_id_7874. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8585_marker[] = "libcgj-batch8585";

/* Soft locale category: LC_COLLATE (string collation order). */
#define B8585_LC_COLLATE  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8585_collate_id(void)
{
	return B8585_LC_COLLATE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lc_collate_id_8585 - soft LC_COLLATE locale category id.
 *
 * Always returns 3. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_lc_collate_id_8585(void)
{
	(void)NULL;
	return b8585_collate_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lc_collate_id_8585(void)
    __attribute__((alias("gj_lc_collate_id_8585")));
