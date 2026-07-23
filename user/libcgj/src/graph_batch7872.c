/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7872: locale category LC_NUMERIC id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_cat_numeric_id_7872(void);
 *     - Return soft LC_NUMERIC category code (1).
 *   uint32_t __gj_locale_cat_numeric_id_7872  (alias)
 *   __libcgj_batch7872_marker = "libcgj-batch7872"
 *
 * Exclusive continuum CREATE-ONLY (7871-7880: locale category stubs —
 * ctype_id, numeric_id, time_id, collate_id, monetary_id, messages_id,
 * all_id, cat_ok, cat_errorish, continuum + batch_id_7880). Unique
 * gj_locale_cat_numeric_id_7872 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7872_marker[] = "libcgj-batch7872";

/* Soft locale category: LC_NUMERIC (decimal point / thousands). */
#define B7872_LC_NUMERIC  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7872_numeric_id(void)
{
	return B7872_LC_NUMERIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_cat_numeric_id_7872 - soft LC_NUMERIC locale category id.
 *
 * Always returns 1. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_locale_cat_numeric_id_7872(void)
{
	(void)NULL;
	return b7872_numeric_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_cat_numeric_id_7872(void)
    __attribute__((alias("gj_locale_cat_numeric_id_7872")));
