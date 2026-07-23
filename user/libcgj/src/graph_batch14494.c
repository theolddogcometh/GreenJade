/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14494: product readiness score tag (wave 14500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_14500(void);
 *     - Returns 0 (product readiness score tag for the milestone 14500
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_14500  (alias)
 *   __libcgj_batch14494_marker = "libcgj-batch14494"
 *
 * Milestone 14500 exclusive continuum CREATE-ONLY (14491-14500). Unique gj_product_score_14500
 * surface only; no multi-def. Distinct from gj_*_14400 / gj_*_14300 / gj_*_14200 milestone
 * surfaces and sibling 14500 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14494_marker[] = "libcgj-batch14494";

/* Product score for wave 14500 (still open). */
#define B14494_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14494_score(void)
{
	return B14494_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_14500 - product readiness score for wave 14500
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_product_score_14500(void)
{
	(void)NULL;
	return b14494_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_14500(void)
    __attribute__((alias("gj_product_score_14500")));
