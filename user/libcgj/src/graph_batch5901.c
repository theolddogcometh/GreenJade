/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5901: product score clamp to [0, 100].
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_clamp_5901(uint32_t score);
 *     - Clamp a product readiness score into the conventional
 *       [0, 100] continuum range. Values above 100 become 100.
 *   uint32_t __gj_product_score_clamp_5901  (alias)
 *   __libcgj_batch5901_marker = "libcgj-batch5901"
 *
 * Exclusive continuum CREATE-ONLY (5901-5910: continuum product score
 * aggregation — clamp_5901, add_sat_5902, avg2_5903, avg3_5904,
 * avg4_5905, weighted_5906, min_5907, max_5908, pct_5909,
 * agg_ready_5910). Unique gj_product_score_clamp_5901 surface only;
 * no multi-def. Distinct from gj_product_score_5900 (batch5898 tag)
 * and gj_product_score_u (batch2998 popcount). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5901_marker[] = "libcgj-batch5901";

/* Conventional product readiness score ceiling. */
#define B5901_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5901_clamp(uint32_t uScore)
{
	if (uScore > B5901_SCORE_MAX) {
		return B5901_SCORE_MAX;
	}
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_clamp_5901 - clamp product score into [0, 100].
 *
 * score: raw product readiness score (any uint32_t domain)
 *
 * Returns score when score <= 100, else 100. Soft pure-data clamp for
 * continuum aggregation inputs. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_clamp_5901(uint32_t uScore)
{
	(void)NULL;
	return b5901_clamp(uScore);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_clamp_5901(uint32_t uScore)
    __attribute__((alias("gj_product_score_clamp_5901")));
