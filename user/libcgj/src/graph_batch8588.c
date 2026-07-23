/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8588: C locale readiness lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_c_ok_u_8588(void);
 *     - Return 1 (soft lamp: the "C" locale is present / ok for this
 *       freestanding continuum). Catalog only; not setlocale(3).
 *   uint32_t __gj_locale_c_ok_u_8588  (alias)
 *   __libcgj_batch8588_marker = "libcgj-batch8588"
 *
 * Exclusive continuum CREATE-ONLY (8581-8590: locale category id stubs —
 * lc_all_id, lc_ctype_id, lc_numeric_id, lc_time_id, lc_collate_id,
 * lc_monetary_id, lc_messages_id, locale_c_ok_u, locale_ready_u,
 * batch_id_8590). Unique gj_locale_c_ok_u_8588 surface only; no multi-def.
 * Distinct from gj_locale_cat_ok_u_7878. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8588_marker[] = "libcgj-batch8588";

/* Soft "C" locale present / ok lamp. */
#define B8588_LOCALE_C_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8588_c_ok(void)
{
	return B8588_LOCALE_C_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_c_ok_u_8588 - soft "C" locale readiness lamp.
 *
 * Always returns 1. Compile-time product tag that the portable C locale
 * catalog is available; does not call setlocale. No parent wires.
 */
uint32_t
gj_locale_c_ok_u_8588(void)
{
	(void)NULL;
	return b8588_c_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_c_ok_u_8588(void)
    __attribute__((alias("gj_locale_c_ok_u_8588")));
