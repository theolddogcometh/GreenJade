/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8294: product readiness score (wave 8300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_8300(void);
 *     - Returns the product readiness score tag for the milestone 8300
 *       continuum (always 8300). Soft compile-time product score tag.
 *   uint32_t __gj_product_score_8300  (alias)
 *   __libcgj_batch8294_marker = "libcgj-batch8294"
 *
 * Milestone 8300 exclusive continuum CREATE-ONLY (8291-8300). Unique
 * gj_product_score_8300 surface only; no multi-def. Distinct from
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

const char __libcgj_batch8294_marker[] = "libcgj-batch8294";

/* Product readiness score tag for wave 8300. */
#define B8294_PRODUCT_SCORE  8300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8294_score(void)
{
	return B8294_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_8300 - report product readiness score for wave 8300.
 *
 * Always returns 8300 (wave-tagged soft product score integer). Soft
 * pure-data only; does not walk filesystems. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_product_score_8300(void)
{
	(void)NULL;
	return b8294_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_8300(void)
    __attribute__((alias("gj_product_score_8300")));
