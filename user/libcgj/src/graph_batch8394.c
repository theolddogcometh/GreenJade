/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8394: product readiness score (wave 8400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_8400(void);
 *     - Returns the product readiness score tag for the milestone 8400
 *       continuum (always 8400). Soft compile-time product score tag.
 *   uint32_t __gj_product_score_8400  (alias)
 *   __libcgj_batch8394_marker = "libcgj-batch8394"
 *
 * Milestone 8400 exclusive continuum CREATE-ONLY (8391-8400). Unique
 * gj_product_score_8400 surface only; no multi-def. Distinct from
 * gj_product_score_8200 (batch8198), gj_product_score_8100 (batch8098),
 * gj_product_score_8000 (batch7998), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8394_marker[] = "libcgj-batch8394";

/* Product readiness score tag for wave 8400. */
#define B8394_PRODUCT_SCORE  8400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8394_score(void)
{
	return B8394_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_8400 - report product readiness score for wave 8400.
 *
 * Always returns 8400 (wave-tagged product score). Soft pure-data
 * only; does not walk filesystems. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_8400(void)
{
	(void)NULL;
	return b8394_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_8400(void)
    __attribute__((alias("gj_product_score_8400")));
