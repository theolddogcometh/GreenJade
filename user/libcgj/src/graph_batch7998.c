/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7998: product readiness score (wave 8000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_8000(void);
 *     - Returns the product readiness score tag for the milestone 8000
 *       continuum (always 8000). Soft compile-time product score tag.
 *   uint32_t __gj_product_score_8000  (alias)
 *   __libcgj_batch7998_marker = "libcgj-batch7998"
 *
 * Milestone 8000 exclusive continuum CREATE-ONLY (7991-8000). Unique
 * gj_product_score_8000 surface only; no multi-def. Distinct from
 * gj_product_score_7000 (batch6998), gj_product_score_6000 (batch5998),
 * gj_product_score_5900 (batch5898), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7998_marker[] = "libcgj-batch7998";

/* Product readiness score tag for wave 8000. */
#define B7998_PRODUCT_SCORE  8000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7998_score(void)
{
	return B7998_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_8000 - report product readiness score for wave 8000.
 *
 * Always returns 8000 (wave-tagged product score). Soft pure-data
 * only; does not walk filesystems. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_8000(void)
{
	(void)NULL;
	return b7998_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_8000(void)
    __attribute__((alias("gj_product_score_8000")));
