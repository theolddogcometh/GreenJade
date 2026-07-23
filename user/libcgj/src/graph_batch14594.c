/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14594: product readiness score tag (wave 14600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_14600(void);
 *     - Returns 0 (product readiness score tag for the milestone 14600
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_14600  (alias)
 *   __libcgj_batch14594_marker = "libcgj-batch14594"
 *
 * Milestone 14600 exclusive continuum CREATE-ONLY (14591-14600). Unique
 * gj_product_score_14600 surface only; no multi-def. Distinct from gj_*_14500 /
 * gj_*_14400 / gj_*_14300 milestone surfaces and sibling 14600 milestone
 * symbols. No parent wires. Soft gates only (no Makefile / map / harness
 * wires in this TU). Product score remains open.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14594_marker[] = "libcgj-batch14594";

/* Product score for wave 14600 (still open). */
#define B14594_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14594_score(void)
{
	return B14594_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_14600 - product readiness score lamp for wave 14600
 *
 * Always returns 0u (still open). Soft pure-data product score tag.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_14600(void)
{
	(void)NULL;
	return b14594_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_14600(void)
    __attribute__((alias("gj_product_score_14600")));
