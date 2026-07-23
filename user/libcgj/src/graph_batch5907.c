/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5907: minimum of two product scores.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_min_5907(uint32_t a, uint32_t b);
 *     - Return the lesser of two product readiness scores after each
 *       is clamped to [0, 100]. Soft pure-data aggregation select.
 *   uint32_t __gj_product_score_min_5907  (alias)
 *   __libcgj_batch5907_marker = "libcgj-batch5907"
 *
 * Exclusive continuum CREATE-ONLY (5901-5910: continuum product score
 * aggregation). Unique gj_product_score_min_5907 surface only; no
 * multi-def. Distinct from gj_product_score_max_5908. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5907_marker[] = "libcgj-batch5907";

#define B5907_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5907_clamp(uint32_t uV)
{
	if (uV > B5907_SCORE_MAX) {
		return B5907_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5907_min(uint32_t uA, uint32_t uB)
{
	uint32_t uCa;
	uint32_t uCb;

	uCa = b5907_clamp(uA);
	uCb = b5907_clamp(uB);
	if (uCa < uCb) {
		return uCa;
	}
	return uCb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_min_5907 - lesser of two clamped product scores.
 *
 * a, b: product readiness component scores
 *
 * Returns min(clamp100(a), clamp100(b)). Self-contained. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_product_score_min_5907(uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b5907_min(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_min_5907(uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_product_score_min_5907")));
