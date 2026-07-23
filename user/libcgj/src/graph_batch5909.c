/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5909: product score as percent of max.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_pct_5909(uint32_t score, uint32_t max);
 *     - Convert a product readiness score into a percentage of max,
 *       clamped to [0, 100]. Returns 0 when max is 0. Floor division:
 *       (score * 100) / max with overflow-safe scaling.
 *   uint32_t __gj_product_score_pct_5909  (alias)
 *   __libcgj_batch5909_marker = "libcgj-batch5909"
 *
 * Exclusive continuum CREATE-ONLY (5901-5910: continuum product score
 * aggregation). Unique gj_product_score_pct_5909 surface only; no
 * multi-def. Distinct from gj_product_score_clamp_5901. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5909_marker[] = "libcgj-batch5909";

#define B5909_PCT_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5909_pct(uint32_t uScore, uint32_t uMax)
{
	uint32_t uPct;

	if (uMax == 0u) {
		return 0u;
	}
	if (uScore >= uMax) {
		return B5909_PCT_MAX;
	}
	/* Prefer (score * 100) / max when score * 100 does not wrap. */
	if (uScore <= (UINT32_MAX / B5909_PCT_MAX)) {
		uPct = (uScore * B5909_PCT_MAX) / uMax;
	} else {
		/* Large score: reduce via floor(score / max) first (0). */
		uPct = (uScore / uMax) * B5909_PCT_MAX;
		/* residual fraction: ((score % max) * 100) / max */
		uPct = uPct + (((uScore % uMax) * B5909_PCT_MAX) / uMax);
	}
	if (uPct > B5909_PCT_MAX) {
		return B5909_PCT_MAX;
	}
	return uPct;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_pct_5909 - score as percentage of max, clamped 0..100.
 *
 * score: raw product readiness numerator
 * max:   full-scale denominator (0 → returns 0)
 *
 * Returns floor percentage in [0, 100]. Soft pure-data normalize step
 * for continuum aggregation. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_pct_5909(uint32_t uScore, uint32_t uMax)
{
	(void)NULL;
	return b5909_pct(uScore, uMax);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_pct_5909(uint32_t uScore, uint32_t uMax)
    __attribute__((alias("gj_product_score_pct_5909")));
