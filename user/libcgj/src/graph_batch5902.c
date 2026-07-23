/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5902: saturating product score add.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_add_sat_5902(uint32_t a, uint32_t b);
 *     - Saturating sum of two product readiness scores, capped at
 *       UINT32_MAX on overflow. Soft pure-integer aggregation step.
 *   uint32_t __gj_product_score_add_sat_5902  (alias)
 *   __libcgj_batch5902_marker = "libcgj-batch5902"
 *
 * Exclusive continuum CREATE-ONLY (5901-5910: continuum product score
 * aggregation). Unique gj_product_score_add_sat_5902 surface only; no
 * multi-def. Distinct from gj_product_score_clamp_5901 and prior
 * saturating add helpers. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5902_marker[] = "libcgj-batch5902";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5902_add_sat(uint32_t uA, uint32_t uB)
{
	uint32_t uSum;

	uSum = uA + uB;
	if (uSum < uA) {
		return UINT32_MAX;
	}
	return uSum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_add_sat_5902 - saturating sum of two product scores.
 *
 * a, b: product readiness component scores
 *
 * Returns a + b, or UINT32_MAX if the sum would wrap. Soft pure-data
 * aggregation. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_add_sat_5902(uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b5902_add_sat(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_add_sat_5902(uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_product_score_add_sat_5902")));
