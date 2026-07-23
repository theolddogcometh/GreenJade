/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7969: bsearch key params ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bsearch_key_params_ok_u_7969(uint32_t key_tag,
 *                                            uint32_t base_tag,
 *                                            uint32_t width,
 *                                            uint32_t cmp_tag);
 *     - Return 1 if key, base, and cmp tags are non-zero and width > 0.
 *   uint32_t __gj_bsearch_key_params_ok_u_7969  (alias)
 *   __libcgj_batch7969_marker = "libcgj-batch7969"
 *
 * Exclusive continuum CREATE-ONLY (7961-7970: bsearch key stubs —
 * key_present_ok, base_present_ok, nmemb_zero_ok, width_ok,
 * cmp_present_ok, cmp_eq_id, cmp_lt_id, cmp_gt_id, params_ok,
 * continuum + batch_id_7970). Unique gj_bsearch_key_params_ok_u_7969
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7969_marker[] = "libcgj-batch7969";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7969_params_ok(uint32_t u32KeyTag, uint32_t u32BaseTag,
    uint32_t u32Width, uint32_t u32CmpTag)
{
	if (u32KeyTag == 0u) {
		return 0u;
	}
	if (u32BaseTag == 0u) {
		return 0u;
	}
	if (u32Width == 0u) {
		return 0u;
	}
	if (u32CmpTag == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_key_params_ok_u_7969 - 1 if soft bsearch key params are ok.
 *
 * key_tag:  soft non-NULL key pointer tag
 * base_tag: soft non-NULL base pointer tag
 * width:    element size (must be > 0)
 * cmp_tag:  soft non-NULL compar tag
 *
 * Returns 1 when all four soft preconditions hold, else 0. Does not
 * inspect nmemb (empty tables remain legal). No parent wires.
 */
uint32_t
gj_bsearch_key_params_ok_u_7969(uint32_t u32KeyTag, uint32_t u32BaseTag,
    uint32_t u32Width, uint32_t u32CmpTag)
{
	(void)NULL;
	return b7969_params_ok(u32KeyTag, u32BaseTag, u32Width, u32CmpTag);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bsearch_key_params_ok_u_7969(uint32_t u32KeyTag,
    uint32_t u32BaseTag, uint32_t u32Width, uint32_t u32CmpTag)
    __attribute__((alias("gj_bsearch_key_params_ok_u_7969")));
