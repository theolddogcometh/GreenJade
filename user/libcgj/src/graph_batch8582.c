/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8582: locale category LC_CTYPE id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lc_ctype_id_8582(void);
 *     - Return soft LC_CTYPE category code (0).
 *   uint32_t __gj_lc_ctype_id_8582  (alias)
 *   __libcgj_batch8582_marker = "libcgj-batch8582"
 *
 * Exclusive continuum CREATE-ONLY (8581-8590: locale category id stubs —
 * lc_all_id, lc_ctype_id, lc_numeric_id, lc_time_id, lc_collate_id,
 * lc_monetary_id, lc_messages_id, locale_c_ok_u, locale_ready_u,
 * batch_id_8590). Unique gj_lc_ctype_id_8582 surface only; no multi-def.
 * Distinct from gj_locale_cat_ctype_id_7871. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8582_marker[] = "libcgj-batch8582";

/* Soft locale category: LC_CTYPE (character classification). */
#define B8582_LC_CTYPE  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8582_ctype_id(void)
{
	return B8582_LC_CTYPE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lc_ctype_id_8582 - soft LC_CTYPE locale category id.
 *
 * Always returns 0. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_lc_ctype_id_8582(void)
{
	(void)NULL;
	return b8582_ctype_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lc_ctype_id_8582(void)
    __attribute__((alias("gj_lc_ctype_id_8582")));
