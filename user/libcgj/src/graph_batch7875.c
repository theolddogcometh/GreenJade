/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7875: locale category LC_MONETARY id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_cat_monetary_id_7875(void);
 *     - Return soft LC_MONETARY category code (4).
 *   uint32_t __gj_locale_cat_monetary_id_7875  (alias)
 *   __libcgj_batch7875_marker = "libcgj-batch7875"
 *
 * Exclusive continuum CREATE-ONLY (7871-7880: locale category stubs —
 * ctype_id, numeric_id, time_id, collate_id, monetary_id, messages_id,
 * all_id, cat_ok, cat_errorish, continuum + batch_id_7880). Unique
 * gj_locale_cat_monetary_id_7875 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7875_marker[] = "libcgj-batch7875";

/* Soft locale category: LC_MONETARY (currency formatting). */
#define B7875_LC_MONETARY  ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7875_monetary_id(void)
{
	return B7875_LC_MONETARY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_cat_monetary_id_7875 - soft LC_MONETARY locale category id.
 *
 * Always returns 4. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_locale_cat_monetary_id_7875(void)
{
	(void)NULL;
	return b7875_monetary_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_cat_monetary_id_7875(void)
    __attribute__((alias("gj_locale_cat_monetary_id_7875")));
