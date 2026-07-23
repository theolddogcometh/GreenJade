/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5904: average of three product scores.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_avg3_5904(uint32_t a, uint32_t b,
 *                                       uint32_t c);
 *     - Floor average of three product scores after each is clamped to
 *       [0, 100]: (clamp(a) + clamp(b) + clamp(c)) / 3.
 *   uint32_t __gj_product_score_avg3_5904  (alias)
 *   __libcgj_batch5904_marker = "libcgj-batch5904"
 *
 * Exclusive continuum CREATE-ONLY (5901-5910: continuum product score
 * aggregation). Unique gj_product_score_avg3_5904 surface only; no
 * multi-def. Distinct from gj_product_score_avg2_5903 and
 * gj_title_ready_score_5726. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5904_marker[] = "libcgj-batch5904";

#define B5904_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5904_clamp(uint32_t uV)
{
	if (uV > B5904_SCORE_MAX) {
		return B5904_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5904_avg3(uint32_t uA, uint32_t uB, uint32_t uC)
{
	uint32_t uSum;

	uSum = b5904_clamp(uA) + b5904_clamp(uB) + b5904_clamp(uC);
	return uSum / 3u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_avg3_5904 - floor average of three clamped scores.
 *
 * a, b, c: product readiness component scores (e.g. smoke/dyn/shell)
 *
 * Returns floor((clamp100(a)+clamp100(b)+clamp100(c))/3). Self-contained.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_avg3_5904(uint32_t uA, uint32_t uB, uint32_t uC)
{
	(void)NULL;
	return b5904_avg3(uA, uB, uC);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_avg3_5904(uint32_t uA, uint32_t uB, uint32_t uC)
    __attribute__((alias("gj_product_score_avg3_5904")));
