/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9394: product readiness score (wave 9400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_9400(void);
 *     - Returns the product readiness score tag for the milestone 9400
 *       continuum (always 9400). Soft compile-time product score tag.
 *   uint32_t __gj_product_score_9400  (alias)
 *   __libcgj_batch9394_marker = "libcgj-batch9394"
 *
 * Milestone 9400 exclusive continuum CREATE-ONLY (9391-9400). Unique
 * gj_product_score_9400 surface only; no multi-def. Distinct from
 * gj_product_score_9300 (batch9294), gj_product_score_9200 (batch9194),
 * gj_product_score_9100 (batch9094), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9394_marker[] = "libcgj-batch9394";

/* Product readiness score tag for wave 9400. */
#define B9394_PRODUCT_SCORE  9400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9394_score(void)
{
	return B9394_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_9400 - report product readiness score for wave 9400.
 *
 * Always returns 9400 (wave-tagged soft product score integer). Soft
 * pure-data only; does not walk filesystems. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_product_score_9400(void)
{
	(void)NULL;
	return b9394_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_9400(void)
    __attribute__((alias("gj_product_score_9400")));
