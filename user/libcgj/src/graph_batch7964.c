/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7964: bsearch element width ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bsearch_width_ok_u_7964(uint32_t width);
 *     - Return 1 if width > 0 (element size usable), else 0.
 *   uint32_t __gj_bsearch_width_ok_u_7964  (alias)
 *   __libcgj_batch7964_marker = "libcgj-batch7964"
 *
 * Exclusive continuum CREATE-ONLY (7961-7970: bsearch key stubs —
 * key_present_ok, base_present_ok, nmemb_zero_ok, width_ok,
 * cmp_present_ok, cmp_eq_id, cmp_lt_id, cmp_gt_id, params_ok,
 * continuum + batch_id_7970). Unique gj_bsearch_width_ok_u_7964
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7964_marker[] = "libcgj-batch7964";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7964_width_ok(uint32_t u32Width)
{
	if (u32Width == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_width_ok_u_7964 - 1 if element size (width) is non-zero.
 *
 * width: soft element size in bytes (size argument of bsearch)
 *
 * Returns 1 when width > 0, else 0. Soft size guard only; does not
 * call libc bsearch. No parent wires.
 */
uint32_t
gj_bsearch_width_ok_u_7964(uint32_t u32Width)
{
	(void)NULL;
	return b7964_width_ok(u32Width);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bsearch_width_ok_u_7964(uint32_t u32Width)
    __attribute__((alias("gj_bsearch_width_ok_u_7964")));
