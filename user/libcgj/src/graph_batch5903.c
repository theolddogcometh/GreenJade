/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5903: average of two product scores.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_avg2_5903(uint32_t a, uint32_t b);
 *     - Floor average of two product scores after each is clamped to
 *       [0, 100]: (clamp(a) + clamp(b)) / 2.
 *   uint32_t __gj_product_score_avg2_5903  (alias)
 *   __libcgj_batch5903_marker = "libcgj-batch5903"
 *
 * Exclusive continuum CREATE-ONLY (5901-5910: continuum product score
 * aggregation). Unique gj_product_score_avg2_5903 surface only; no
 * multi-def. Distinct from gj_product_score_add_sat_5902. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5903_marker[] = "libcgj-batch5903";

#define B5903_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5903_clamp(uint32_t uV)
{
	if (uV > B5903_SCORE_MAX) {
		return B5903_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5903_avg2(uint32_t uA, uint32_t uB)
{
	uint32_t uCa;
	uint32_t uCb;

	uCa = b5903_clamp(uA);
	uCb = b5903_clamp(uB);
	return (uCa + uCb) / 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_avg2_5903 - floor average of two clamped scores.
 *
 * a, b: product readiness component scores
 *
 * Returns floor((clamp100(a) + clamp100(b)) / 2). Self-contained clamp
 * (no parent wires). Does not call libc.
 */
uint32_t
gj_product_score_avg2_5903(uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b5903_avg2(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_avg2_5903(uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_product_score_avg2_5903")));
