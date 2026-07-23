/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5905: average of four product scores.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_avg4_5905(uint32_t a, uint32_t b,
 *                                       uint32_t c, uint32_t d);
 *     - Floor average of four product scores after each is clamped to
 *       [0, 100]. Covers the classic four readiness slots
 *       (path/shell/libcgj/extra or smoke/dyn/shell/libcgj).
 *   uint32_t __gj_product_score_avg4_5905  (alias)
 *   __libcgj_batch5905_marker = "libcgj-batch5905"
 *
 * Exclusive continuum CREATE-ONLY (5901-5910: continuum product score
 * aggregation). Unique gj_product_score_avg4_5905 surface only; no
 * multi-def. Distinct from gj_product_score_avg3_5904 and
 * gj_product_score_u (batch2998 popcount). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5905_marker[] = "libcgj-batch5905";

#define B5905_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5905_clamp(uint32_t uV)
{
	if (uV > B5905_SCORE_MAX) {
		return B5905_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5905_avg4(uint32_t uA, uint32_t uB, uint32_t uC, uint32_t uD)
{
	uint32_t uSum;

	uSum = b5905_clamp(uA) + b5905_clamp(uB)
	    + b5905_clamp(uC) + b5905_clamp(uD);
	return uSum / 4u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_avg4_5905 - floor average of four clamped scores.
 *
 * a..d: four product readiness component scores
 *
 * Returns floor(sum(clamp100(ci)) / 4). Self-contained. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_product_score_avg4_5905(uint32_t uA, uint32_t uB, uint32_t uC,
    uint32_t uD)
{
	(void)NULL;
	return b5905_avg4(uA, uB, uC, uD);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_avg4_5905(uint32_t uA, uint32_t uB, uint32_t uC,
    uint32_t uD)
    __attribute__((alias("gj_product_score_avg4_5905")));
