/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13294: product readiness score (wave 13300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_13300(void);
 *     - Returns 0 (product readiness score tag for the milestone 13300
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_13300  (alias)
 *   __libcgj_batch13294_marker = "libcgj-batch13294"
 *
 * Milestone 13300 exclusive continuum CREATE-ONLY (13291-13300). Unique
 * gj_product_score_13300 surface only; no multi-def. Distinct from
 * gj_product_score_13200 (batch13194), gj_product_score_13100
 * (batch13094), gj_product_score_13000 (batch12994), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13294_marker[] = "libcgj-batch13294";

/* Product readiness score tag for wave 13300 (still open). */
#define B13294_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13294_score(void)
{
	return B13294_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_13300 - report product readiness score for wave 13300.
 *
 * Always returns 0 (score still open / not scored). Soft pure-data
 * only; does not walk filesystems. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_13300(void)
{
	(void)NULL;
	return b13294_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_13300(void)
    __attribute__((alias("gj_product_score_13300")));
