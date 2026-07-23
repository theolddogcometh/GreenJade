/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7963: bsearch nmemb zero ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bsearch_nmemb_zero_ok_u_7963(uint32_t n);
 *     - Return 1 (soft: empty tables / any nmemb are legal for bsearch).
 *   uint32_t __gj_bsearch_nmemb_zero_ok_u_7963  (alias)
 *   __libcgj_batch7963_marker = "libcgj-batch7963"
 *
 * Exclusive continuum CREATE-ONLY (7961-7970: bsearch key stubs —
 * key_present_ok, base_present_ok, nmemb_zero_ok, width_ok,
 * cmp_present_ok, cmp_eq_id, cmp_lt_id, cmp_gt_id, params_ok,
 * continuum + batch_id_7970). Unique gj_bsearch_nmemb_zero_ok_u_7963
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7963_marker[] = "libcgj-batch7963";

/* Soft: every nmemb value (including 0) is accepted. */
#define B7963_NMEMB_OK  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7963_nmemb_zero_ok(uint32_t u32N)
{
	(void)u32N;
	return B7963_NMEMB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_nmemb_zero_ok_u_7963 - soft nmemb legality (always 1).
 *
 * n: element count (including 0 for empty table)
 *
 * Always returns 1. Classic bsearch accepts n == 0 (immediate miss).
 * Soft catalog only; no parent wires.
 */
uint32_t
gj_bsearch_nmemb_zero_ok_u_7963(uint32_t u32N)
{
	(void)NULL;
	return b7963_nmemb_zero_ok(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bsearch_nmemb_zero_ok_u_7963(uint32_t u32N)
    __attribute__((alias("gj_bsearch_nmemb_zero_ok_u_7963")));
