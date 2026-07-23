/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7968: bsearch key compar greater-than id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bsearch_key_cmp_gt_id_7968(void);
 *     - Return soft compar greater-than branch id (2).
 *   uint32_t __gj_bsearch_key_cmp_gt_id_7968  (alias)
 *   __libcgj_batch7968_marker = "libcgj-batch7968"
 *
 * Exclusive continuum CREATE-ONLY (7961-7970: bsearch key stubs —
 * key_present_ok, base_present_ok, nmemb_zero_ok, width_ok,
 * cmp_present_ok, cmp_eq_id, cmp_lt_id, cmp_gt_id, params_ok,
 * continuum + batch_id_7970). Unique gj_bsearch_key_cmp_gt_id_7968
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7968_marker[] = "libcgj-batch7968";

/* Soft catalog id for "key > mid element" branch. */
#define B7968_CMP_GT_ID  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7968_cmp_gt_id(void)
{
	return B7968_CMP_GT_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_key_cmp_gt_id_7968 - soft compar greater-than branch id.
 *
 * Always returns 2. Catalog tag for the key-greater-than-element path;
 * does not call compar or libc bsearch. No parent wires.
 */
uint32_t
gj_bsearch_key_cmp_gt_id_7968(void)
{
	(void)NULL;
	return b7968_cmp_gt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bsearch_key_cmp_gt_id_7968(void)
    __attribute__((alias("gj_bsearch_key_cmp_gt_id_7968")));
