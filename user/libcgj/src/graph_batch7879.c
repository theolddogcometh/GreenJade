/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7879: locale category errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_cat_errorish_u_7879(uint32_t cat);
 *     - Return 1 if cat is not a known soft locale category
 *       (not ctype..all / 0..6), else 0.
 *   uint32_t __gj_locale_cat_errorish_u_7879  (alias)
 *   __libcgj_batch7879_marker = "libcgj-batch7879"
 *
 * Exclusive continuum CREATE-ONLY (7871-7880: locale category stubs —
 * ctype_id, numeric_id, time_id, collate_id, monetary_id, messages_id,
 * all_id, cat_ok, cat_errorish, continuum + batch_id_7880). Unique
 * gj_locale_cat_errorish_u_7879 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7879_marker[] = "libcgj-batch7879";

/* Soft known locale categories span 0 (LC_CTYPE) through 6 (LC_ALL). */
#define B7879_LC_MAX  ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7879_cat_errorish(uint32_t u32Cat)
{
	/* u32Cat is unsigned; known range is 0..6 inclusive. */
	if (u32Cat <= B7879_LC_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_cat_errorish_u_7879 - 1 if cat is not a known locale category.
 *
 * cat: soft locale category code
 *
 * Returns 1 when cat is outside 0..6; else 0. Soft catalog check; does
 * not call setlocale. No parent wires.
 */
uint32_t
gj_locale_cat_errorish_u_7879(uint32_t u32Cat)
{
	(void)NULL;
	return b7879_cat_errorish(u32Cat);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_cat_errorish_u_7879(uint32_t u32Cat)
    __attribute__((alias("gj_locale_cat_errorish_u_7879")));
