/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10294: product readiness score (wave 10300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_10300(void);
 *     - Returns 0 (product readiness score lamp still open for the
 *       milestone 10300 continuum soft gates). Soft compile-time
 *       product score tag.
 *   uint32_t __gj_product_score_10300  (alias)
 *   __libcgj_batch10294_marker = "libcgj-batch10294"
 *
 * Milestone 10300 exclusive continuum CREATE-ONLY (10291-10300). Unique
 * gj_product_score_10300 surface only; no multi-def. Distinct from
 * gj_product_score_10200 (batch10194), gj_product_score_10100
 * (batch10094), gj_product_score_10000 (batch9994), and
 * gj_product_score_u (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10294_marker[] = "libcgj-batch10294";

/* Product readiness score lamp for wave 10300 (soft open / zero). */
#define B10294_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10294_score(void)
{
	return B10294_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_10300 - report product readiness score for wave 10300.
 *
 * Always returns 0 (soft product score still open). Soft pure-data
 * only; does not walk filesystems. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_10300(void)
{
	(void)NULL;
	return b10294_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_10300(void)
    __attribute__((alias("gj_product_score_10300")));
