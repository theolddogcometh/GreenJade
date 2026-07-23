/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8581: locale category LC_ALL id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lc_all_id_8581(void);
 *     - Return soft LC_ALL category code (6).
 *   uint32_t __gj_lc_all_id_8581  (alias)
 *   __libcgj_batch8581_marker = "libcgj-batch8581"
 *
 * Exclusive continuum CREATE-ONLY (8581-8590: locale category id stubs —
 * lc_all_id, lc_ctype_id, lc_numeric_id, lc_time_id, lc_collate_id,
 * lc_monetary_id, lc_messages_id, locale_c_ok_u, locale_ready_u,
 * batch_id_8590). Unique gj_lc_all_id_8581 surface only; no multi-def.
 * Distinct from gj_locale_cat_all_id_7877. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8581_marker[] = "libcgj-batch8581";

/* Soft locale category: LC_ALL (all categories together). */
#define B8581_LC_ALL  ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8581_all_id(void)
{
	return B8581_LC_ALL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lc_all_id_8581 - soft LC_ALL locale category id.
 *
 * Always returns 6. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_lc_all_id_8581(void)
{
	(void)NULL;
	return b8581_all_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lc_all_id_8581(void)
    __attribute__((alias("gj_lc_all_id_8581")));
