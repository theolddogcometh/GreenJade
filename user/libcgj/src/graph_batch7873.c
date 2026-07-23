/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7873: locale category LC_TIME id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_cat_time_id_7873(void);
 *     - Return soft LC_TIME category code (2).
 *   uint32_t __gj_locale_cat_time_id_7873  (alias)
 *   __libcgj_batch7873_marker = "libcgj-batch7873"
 *
 * Exclusive continuum CREATE-ONLY (7871-7880: locale category stubs —
 * ctype_id, numeric_id, time_id, collate_id, monetary_id, messages_id,
 * all_id, cat_ok, cat_errorish, continuum + batch_id_7880). Unique
 * gj_locale_cat_time_id_7873 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7873_marker[] = "libcgj-batch7873";

/* Soft locale category: LC_TIME (date / time formats). */
#define B7873_LC_TIME  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7873_time_id(void)
{
	return B7873_LC_TIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_cat_time_id_7873 - soft LC_TIME locale category id.
 *
 * Always returns 2. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_locale_cat_time_id_7873(void)
{
	(void)NULL;
	return b7873_time_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_cat_time_id_7873(void)
    __attribute__((alias("gj_locale_cat_time_id_7873")));
