/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7967: bsearch key compar less-than id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bsearch_key_cmp_lt_id_7967(void);
 *     - Return soft compar less-than branch id (1).
 *   uint32_t __gj_bsearch_key_cmp_lt_id_7967  (alias)
 *   __libcgj_batch7967_marker = "libcgj-batch7967"
 *
 * Exclusive continuum CREATE-ONLY (7961-7970: bsearch key stubs —
 * key_present_ok, base_present_ok, nmemb_zero_ok, width_ok,
 * cmp_present_ok, cmp_eq_id, cmp_lt_id, cmp_gt_id, params_ok,
 * continuum + batch_id_7970). Unique gj_bsearch_key_cmp_lt_id_7967
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7967_marker[] = "libcgj-batch7967";

/* Soft catalog id for "key < mid element" branch (not a signed -1). */
#define B7967_CMP_LT_ID  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7967_cmp_lt_id(void)
{
	return B7967_CMP_LT_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_key_cmp_lt_id_7967 - soft compar less-than branch id.
 *
 * Always returns 1. Catalog tag for the key-less-than-element path;
 * does not call compar or libc bsearch. No parent wires.
 */
uint32_t
gj_bsearch_key_cmp_lt_id_7967(void)
{
	(void)NULL;
	return b7967_cmp_lt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bsearch_key_cmp_lt_id_7967(void)
    __attribute__((alias("gj_bsearch_key_cmp_lt_id_7967")));
