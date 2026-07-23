/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8584: locale category LC_TIME id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lc_time_id_8584(void);
 *     - Return soft LC_TIME category code (2).
 *   uint32_t __gj_lc_time_id_8584  (alias)
 *   __libcgj_batch8584_marker = "libcgj-batch8584"
 *
 * Exclusive continuum CREATE-ONLY (8581-8590: locale category id stubs —
 * lc_all_id, lc_ctype_id, lc_numeric_id, lc_time_id, lc_collate_id,
 * lc_monetary_id, lc_messages_id, locale_c_ok_u, locale_ready_u,
 * batch_id_8590). Unique gj_lc_time_id_8584 surface only; no multi-def.
 * Distinct from gj_locale_cat_time_id_7873. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8584_marker[] = "libcgj-batch8584";

/* Soft locale category: LC_TIME (date / time formats). */
#define B8584_LC_TIME  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8584_time_id(void)
{
	return B8584_LC_TIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lc_time_id_8584 - soft LC_TIME locale category id.
 *
 * Always returns 2. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_lc_time_id_8584(void)
{
	(void)NULL;
	return b8584_time_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lc_time_id_8584(void)
    __attribute__((alias("gj_lc_time_id_8584")));
