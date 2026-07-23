/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7878: locale category validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_cat_ok_u_7878(uint32_t cat);
 *     - Return 1 if cat is a known soft locale category
 *       (ctype|numeric|time|collate|monetary|messages|all), else 0.
 *   uint32_t __gj_locale_cat_ok_u_7878  (alias)
 *   __libcgj_batch7878_marker = "libcgj-batch7878"
 *
 * Exclusive continuum CREATE-ONLY (7871-7880: locale category stubs —
 * ctype_id, numeric_id, time_id, collate_id, monetary_id, messages_id,
 * all_id, cat_ok, cat_errorish, continuum + batch_id_7880). Unique
 * gj_locale_cat_ok_u_7878 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7878_marker[] = "libcgj-batch7878";

/* Soft known locale categories: LC_CTYPE..LC_ALL map to 0..6. */
#define B7878_LC_CTYPE     ((uint32_t)0u)
#define B7878_LC_NUMERIC   ((uint32_t)1u)
#define B7878_LC_TIME      ((uint32_t)2u)
#define B7878_LC_COLLATE   ((uint32_t)3u)
#define B7878_LC_MONETARY  ((uint32_t)4u)
#define B7878_LC_MESSAGES  ((uint32_t)5u)
#define B7878_LC_ALL       ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7878_cat_ok(uint32_t u32Cat)
{
	if (u32Cat == B7878_LC_CTYPE) {
		return 1u;
	}
	if (u32Cat == B7878_LC_NUMERIC) {
		return 1u;
	}
	if (u32Cat == B7878_LC_TIME) {
		return 1u;
	}
	if (u32Cat == B7878_LC_COLLATE) {
		return 1u;
	}
	if (u32Cat == B7878_LC_MONETARY) {
		return 1u;
	}
	if (u32Cat == B7878_LC_MESSAGES) {
		return 1u;
	}
	if (u32Cat == B7878_LC_ALL) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_cat_ok_u_7878 - 1 if cat is a known soft locale category.
 *
 * cat: soft locale category code
 *
 * Returns 1 when cat is 0..6 (ctype..all); else 0. Soft catalog check;
 * does not call setlocale. No parent wires.
 */
uint32_t
gj_locale_cat_ok_u_7878(uint32_t u32Cat)
{
	(void)NULL;
	return b7878_cat_ok(u32Cat);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_cat_ok_u_7878(uint32_t u32Cat)
    __attribute__((alias("gj_locale_cat_ok_u_7878")));
