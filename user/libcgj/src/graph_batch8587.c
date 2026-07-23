/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8587: locale category LC_MESSAGES id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lc_messages_id_8587(void);
 *     - Return soft LC_MESSAGES category code (5).
 *   uint32_t __gj_lc_messages_id_8587  (alias)
 *   __libcgj_batch8587_marker = "libcgj-batch8587"
 *
 * Exclusive continuum CREATE-ONLY (8581-8590: locale category id stubs —
 * lc_all_id, lc_ctype_id, lc_numeric_id, lc_time_id, lc_collate_id,
 * lc_monetary_id, lc_messages_id, locale_c_ok_u, locale_ready_u,
 * batch_id_8590). Unique gj_lc_messages_id_8587 surface only; no multi-def.
 * Distinct from gj_locale_cat_messages_id_7876. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8587_marker[] = "libcgj-batch8587";

/* Soft locale category: LC_MESSAGES (affirmative / negative strings). */
#define B8587_LC_MESSAGES  ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8587_messages_id(void)
{
	return B8587_LC_MESSAGES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lc_messages_id_8587 - soft LC_MESSAGES locale category id.
 *
 * Always returns 5. Catalog id only; does not call setlocale. No parent
 * wires.
 */
uint32_t
gj_lc_messages_id_8587(void)
{
	(void)NULL;
	return b8587_messages_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lc_messages_id_8587(void)
    __attribute__((alias("gj_lc_messages_id_8587")));
