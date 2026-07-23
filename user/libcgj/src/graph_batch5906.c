/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5906: weighted product score blend.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_weighted_5906(uint32_t a, uint32_t b,
 *                                           uint32_t wa, uint32_t wb);
 *     - Weighted floor blend of two product scores after clamp to
 *       [0, 100]: (clamp(a)*wa + clamp(b)*wb) / (wa+wb). Returns 0
 *       when both weights are zero. Weights are treated as soft
 *       relative shares (large weights fold via division-first path).
 *   uint32_t __gj_product_score_weighted_5906  (alias)
 *   __libcgj_batch5906_marker = "libcgj-batch5906"
 *
 * Exclusive continuum CREATE-ONLY (5901-5910: continuum product score
 * aggregation). Unique gj_product_score_weighted_5906 surface only; no
 * multi-def. Distinct from gj_product_score_avg2_5903. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5906_marker[] = "libcgj-batch5906";

#define B5906_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5906_clamp(uint32_t uV)
{
	if (uV > B5906_SCORE_MAX) {
		return B5906_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5906_weighted(uint32_t uA, uint32_t uB, uint32_t uWa, uint32_t uWb)
{
	uint32_t uCa;
	uint32_t uCb;
	uint32_t uDen;
	uint32_t uTermA;
	uint32_t uTermB;
	uint32_t uNum;

	uDen = uWa + uWb;
	if (uDen == 0u) {
		return 0u;
	}
	/* Denominator wrap: treat as full-scale pass of larger weight. */
	if (uDen < uWa) {
		uCa = b5906_clamp(uA);
		uCb = b5906_clamp(uB);
		if (uWa >= uWb) {
			return uCa;
		}
		return uCb;
	}
	uCa = b5906_clamp(uA);
	uCb = b5906_clamp(uB);

	/* Safe multiply: ca,cb <= 100 so need wa <= UINT32_MAX/100. */
	if ((uWa != 0u && uWa > (UINT32_MAX / B5906_SCORE_MAX))
	    || (uWb != 0u && uWb > (UINT32_MAX / B5906_SCORE_MAX))) {
		/* Division-first approximate blend for huge weights. */
		return ((uCa * (uWa / uDen)) + (uCb * (uWb / uDen)));
	}
	uTermA = uCa * uWa;
	uTermB = uCb * uWb;
	uNum = uTermA + uTermB;
	if (uNum < uTermA) {
		return B5906_SCORE_MAX;
	}
	return uNum / uDen;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_weighted_5906 - weighted floor blend of two scores.
 *
 * a, b:   product readiness component scores (clamped to [0, 100])
 * wa, wb: non-negative integer weights; both zero yields 0
 *
 * Returns floor((clamp(a)*wa + clamp(b)*wb) / (wa+wb)) with overflow
 * guards. Soft pure-data aggregation. No parent wires.
 */
uint32_t
gj_product_score_weighted_5906(uint32_t uA, uint32_t uB, uint32_t uWa,
    uint32_t uWb)
{
	(void)NULL;
	return b5906_weighted(uA, uB, uWa, uWb);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_weighted_5906(uint32_t uA, uint32_t uB,
    uint32_t uWa, uint32_t uWb)
    __attribute__((alias("gj_product_score_weighted_5906")));
