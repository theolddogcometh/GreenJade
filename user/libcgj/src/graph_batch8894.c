/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8894: product readiness score (wave 8900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_8900(void);
 *     - Returns the product readiness score tag for the milestone 8900
 *       continuum (always 8900). Soft compile-time product score tag.
 *   uint32_t __gj_product_score_8900  (alias)
 *   __libcgj_batch8894_marker = "libcgj-batch8894"
 *
 * Milestone 8900 exclusive continuum CREATE-ONLY (8891-8900). Unique
 * gj_product_score_8900 surface only; no multi-def. Distinct from
 * gj_product_score_8800 (batch8794), gj_product_score_8700 (batch8694),
 * gj_product_score_8600 (batch8594), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8894_marker[] = "libcgj-batch8894";

/* Product readiness score tag for wave 8900. */
#define B8894_PRODUCT_SCORE  8900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8894_score(void)
{
	return B8894_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_8900 - report product readiness score for wave 8900.
 *
 * Always returns 8900 (wave-tagged soft product score integer). Soft
 * pure-data only; does not walk filesystems. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_product_score_8900(void)
{
	(void)NULL;
	return b8894_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_8900(void)
    __attribute__((alias("gj_product_score_8900")));
