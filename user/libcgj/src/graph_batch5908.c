/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5908: maximum of two product scores.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_max_5908(uint32_t a, uint32_t b);
 *     - Return the greater of two product readiness scores after each
 *       is clamped to [0, 100]. Soft pure-data aggregation select.
 *   uint32_t __gj_product_score_max_5908  (alias)
 *   __libcgj_batch5908_marker = "libcgj-batch5908"
 *
 * Exclusive continuum CREATE-ONLY (5901-5910: continuum product score
 * aggregation). Unique gj_product_score_max_5908 surface only; no
 * multi-def. Distinct from gj_product_score_min_5907. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5908_marker[] = "libcgj-batch5908";

#define B5908_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5908_clamp(uint32_t uV)
{
	if (uV > B5908_SCORE_MAX) {
		return B5908_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5908_max(uint32_t uA, uint32_t uB)
{
	uint32_t uCa;
	uint32_t uCb;

	uCa = b5908_clamp(uA);
	uCb = b5908_clamp(uB);
	if (uCa > uCb) {
		return uCa;
	}
	return uCb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_max_5908 - greater of two clamped product scores.
 *
 * a, b: product readiness component scores
 *
 * Returns max(clamp100(a), clamp100(b)). Self-contained. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_product_score_max_5908(uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b5908_max(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_max_5908(uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_product_score_max_5908")));
