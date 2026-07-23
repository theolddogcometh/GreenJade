/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7874: locale category LC_COLLATE id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_cat_collate_id_7874(void);
 *     - Return soft LC_COLLATE category code (3).
 *   uint32_t __gj_locale_cat_collate_id_7874  (alias)
 *   __libcgj_batch7874_marker = "libcgj-batch7874"
 *
 * Exclusive continuum CREATE-ONLY (7871-7880: locale category stubs —
 * ctype_id, numeric_id, time_id, collate_id, monetary_id, messages_id,
 * all_id, cat_ok, cat_errorish, continuum + batch_id_7880). Unique
 * gj_locale_cat_collate_id_7874 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7874_marker[] = "libcgj-batch7874";

/* Soft locale category: LC_COLLATE (string collation order). */
#define B7874_LC_COLLATE  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7874_collate_id(void)
{
	return B7874_LC_COLLATE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_cat_collate_id_7874 - soft LC_COLLATE locale category id.
 *
 * Always returns 3. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_locale_cat_collate_id_7874(void)
{
	(void)NULL;
	return b7874_collate_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_cat_collate_id_7874(void)
    __attribute__((alias("gj_locale_cat_collate_id_7874")));
