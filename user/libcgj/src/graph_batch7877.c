/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7877: locale category LC_ALL id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_cat_all_id_7877(void);
 *     - Return soft LC_ALL category code (6).
 *   uint32_t __gj_locale_cat_all_id_7877  (alias)
 *   __libcgj_batch7877_marker = "libcgj-batch7877"
 *
 * Exclusive continuum CREATE-ONLY (7871-7880: locale category stubs —
 * ctype_id, numeric_id, time_id, collate_id, monetary_id, messages_id,
 * all_id, cat_ok, cat_errorish, continuum + batch_id_7880). Unique
 * gj_locale_cat_all_id_7877 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7877_marker[] = "libcgj-batch7877";

/* Soft locale category: LC_ALL (all categories together). */
#define B7877_LC_ALL  ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7877_all_id(void)
{
	return B7877_LC_ALL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_cat_all_id_7877 - soft LC_ALL locale category id.
 *
 * Always returns 6. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_locale_cat_all_id_7877(void)
{
	(void)NULL;
	return b7877_all_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_cat_all_id_7877(void)
    __attribute__((alias("gj_locale_cat_all_id_7877")));
