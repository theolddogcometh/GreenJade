/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9594: product readiness score (wave 9600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_9600(void);
 *     - Returns the product readiness score tag for the milestone 9600
 *       continuum (always 9600). Soft compile-time product score tag.
 *   uint32_t __gj_product_score_9600  (alias)
 *   __libcgj_batch9594_marker = "libcgj-batch9594"
 *
 * Milestone 9600 exclusive continuum CREATE-ONLY (9591-9600). Unique
 * gj_product_score_9600 surface only; no multi-def. Distinct from
 * gj_product_score_9500 (batch9494), gj_product_score_9400 (batch9394),
 * gj_product_score_9300 (batch9294), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9594_marker[] = "libcgj-batch9594";

/* Product readiness score tag for wave 9600. */
#define B9594_PRODUCT_SCORE  9600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9594_score(void)
{
	return B9594_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_9600 - report product readiness score for wave 9600.
 *
 * Always returns 9600 (wave-tagged soft product score integer). Soft
 * pure-data only; does not walk filesystems. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_product_score_9600(void)
{
	(void)NULL;
	return b9594_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_9600(void)
    __attribute__((alias("gj_product_score_9600")));
