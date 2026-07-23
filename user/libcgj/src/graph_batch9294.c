/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9294: product readiness score (wave 9300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_9300(void);
 *     - Returns the product readiness score tag for the milestone 9300
 *       continuum (always 9300). Soft compile-time product score tag.
 *   uint32_t __gj_product_score_9300  (alias)
 *   __libcgj_batch9294_marker = "libcgj-batch9294"
 *
 * Milestone 9300 exclusive continuum CREATE-ONLY (9291-9300). Unique
 * gj_product_score_9300 surface only; no multi-def. Distinct from
 * gj_product_score_9200 (batch9194), gj_product_score_9100 (batch9094),
 * gj_product_score_9000 (batch8994), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9294_marker[] = "libcgj-batch9294";

/* Product readiness score tag for wave 9300. */
#define B9294_PRODUCT_SCORE  9300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9294_score(void)
{
	return B9294_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_9300 - report product readiness score for wave 9300.
 *
 * Always returns 9300 (wave-tagged soft product score integer). Soft
 * pure-data only; does not walk filesystems. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_product_score_9300(void)
{
	(void)NULL;
	return b9294_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_9300(void)
    __attribute__((alias("gj_product_score_9300")));
