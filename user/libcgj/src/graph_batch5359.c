/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5359: product-path score tag (prior milestone).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_path_score_u(void);
 *     - Returns 5350 (product-path score anchored at prior milestone).
 *       Soft compile-time product continuum tag.
 *   uint32_t __gj_product_path_score_u  (alias)
 *   __libcgj_batch5359_marker = "libcgj-batch5359"
 *
 * Exclusive continuum CREATE-ONLY (5351-5360: Steam/Deck product path
 * helpers unique). Unique gj_product_path_score_u surface only; no
 * multi-def. Distinct from gj_product_score_5350 (batch5348) and
 * gj_product_score_u (batch2998). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5359_marker[] = "libcgj-batch5359";

/* Product-path score anchored at milestone 5350. */
#define B5359_PRODUCT_PATH_SCORE  5350u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5359_product_path_score(void)
{
	return B5359_PRODUCT_PATH_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_path_score_u - report product-path score for this continuum.
 *
 * Always returns 5350 (prior milestone product-path score). Soft
 * pure-data only; does not walk product trees. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_product_path_score_u(void)
{
	(void)NULL;
	return b5359_product_path_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_path_score_u(void)
    __attribute__((alias("gj_product_path_score_u")));
