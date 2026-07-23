/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6298: product readiness score (wave 6300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_6300(void);
 *     - Returns the product readiness score tag for the milestone 6300
 *       continuum (always 6300). Soft compile-time product score tag.
 *   uint32_t __gj_product_score_6300  (alias)
 *   __libcgj_batch6298_marker = "libcgj-batch6298"
 *
 * Milestone 6300 exclusive continuum CREATE-ONLY (6291-6300). Unique
 * gj_product_score_6300 surface only; no multi-def. Distinct from
 * gj_product_score_6200 (batch6198), gj_product_score_6100 (batch6098),
 * gj_product_score_6000 (batch5998), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6298_marker[] = "libcgj-batch6298";

/* Product readiness score tag for wave 6300. */
#define B6298_PRODUCT_SCORE  6300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6298_score(void)
{
	return B6298_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_6300 - report product readiness score for wave 6300.
 *
 * Always returns 6300 (wave-tagged product score). Soft pure-data
 * only; does not walk filesystems. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_6300(void)
{
	(void)NULL;
	return b6298_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_6300(void)
    __attribute__((alias("gj_product_score_6300")));
