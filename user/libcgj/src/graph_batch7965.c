/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7965: bsearch compar present ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bsearch_cmp_present_ok_u_7965(uint32_t cmp_tag);
 *     - Return 1 if cmp_tag is non-zero (compar soft-present), else 0.
 *   uint32_t __gj_bsearch_cmp_present_ok_u_7965  (alias)
 *   __libcgj_batch7965_marker = "libcgj-batch7965"
 *
 * Exclusive continuum CREATE-ONLY (7961-7970: bsearch key stubs —
 * key_present_ok, base_present_ok, nmemb_zero_ok, width_ok,
 * cmp_present_ok, cmp_eq_id, cmp_lt_id, cmp_gt_id, params_ok,
 * continuum + batch_id_7970). Unique gj_bsearch_cmp_present_ok_u_7965
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7965_marker[] = "libcgj-batch7965";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7965_cmp_present_ok(uint32_t u32CmpTag)
{
	if (u32CmpTag == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_cmp_present_ok_u_7965 - 1 if soft compar tag is non-zero.
 *
 * cmp_tag: soft stand-in for a non-NULL compar function (0 = absent)
 *
 * Returns 1 when cmp_tag != 0, else 0. Soft compar guard only; does
 * not call libc bsearch. No parent wires.
 */
uint32_t
gj_bsearch_cmp_present_ok_u_7965(uint32_t u32CmpTag)
{
	(void)NULL;
	return b7965_cmp_present_ok(u32CmpTag);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bsearch_cmp_present_ok_u_7965(uint32_t u32CmpTag)
    __attribute__((alias("gj_bsearch_cmp_present_ok_u_7965")));
