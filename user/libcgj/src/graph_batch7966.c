/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7966: bsearch key compar equal id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bsearch_key_cmp_eq_id_7966(void);
 *     - Return soft compar equal result id (0).
 *   uint32_t __gj_bsearch_key_cmp_eq_id_7966  (alias)
 *   __libcgj_batch7966_marker = "libcgj-batch7966"
 *
 * Exclusive continuum CREATE-ONLY (7961-7970: bsearch key stubs —
 * key_present_ok, base_present_ok, nmemb_zero_ok, width_ok,
 * cmp_present_ok, cmp_eq_id, cmp_lt_id, cmp_gt_id, params_ok,
 * continuum + batch_id_7970). Unique gj_bsearch_key_cmp_eq_id_7966
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7966_marker[] = "libcgj-batch7966";

/* Soft compar equal: classic bsearch/qsort compar returns 0 on match. */
#define B7966_CMP_EQ  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7966_cmp_eq_id(void)
{
	return B7966_CMP_EQ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_key_cmp_eq_id_7966 - soft compar equal result id.
 *
 * Always returns 0. Catalog id only (key matches element); does not
 * call compar or libc bsearch. No parent wires.
 */
uint32_t
gj_bsearch_key_cmp_eq_id_7966(void)
{
	(void)NULL;
	return b7966_cmp_eq_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bsearch_key_cmp_eq_id_7966(void)
    __attribute__((alias("gj_bsearch_key_cmp_eq_id_7966")));
