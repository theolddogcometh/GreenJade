/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8594: product readiness score (wave 8600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_8600(void);
 *     - Returns the product readiness score tag for the milestone 8600
 *       continuum (always 8600). Soft compile-time product score tag.
 *   uint32_t __gj_product_score_8600  (alias)
 *   __libcgj_batch8594_marker = "libcgj-batch8594"
 *
 * Milestone 8600 exclusive continuum CREATE-ONLY (8591-8600). Unique
 * gj_product_score_8600 surface only; no multi-def. Distinct from
 * gj_product_score_8500 (batch8494), gj_product_score_8400 (batch8394),
 * gj_product_score_8300 (batch8294), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8594_marker[] = "libcgj-batch8594";

/* Product readiness score tag for wave 8600. */
#define B8594_PRODUCT_SCORE  8600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8594_score(void)
{
	return B8594_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_8600 - report product readiness score for wave 8600.
 *
 * Always returns 8600 (wave-tagged soft product score integer). Soft
 * pure-data only; does not walk filesystems. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_product_score_8600(void)
{
	(void)NULL;
	return b8594_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_8600(void)
    __attribute__((alias("gj_product_score_8600")));
