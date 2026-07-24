/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25294: product readiness score tag (wave 25300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_25300(void);
 *     - Returns 0 (product readiness score tag for the milestone 25300
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_25300  (alias)
 *   __libcgj_batch25294_marker = "libcgj-batch25294"
 *
 * Milestone 25300 exclusive continuum CREATE-ONLY (25291-25300). Unique
 * gj_product_score_25300 surface only; no multi-def. Distinct from
 * gj_product_score_25200 / gj_product_score_25100 / gj_product_score_25000
 * and sibling 25300 milestone symbols.
 * No parent wires. No __int128.
 *
 * HONESTY: product_score remains intentionally 0. Soft continuum graph
 * growth is not a product readiness claim; score stays open until a
 * real product-score track advances it.
 *
 * CGJ soft marker band (25291–25300): host soft probes
 * (cgj_soft_milestone_25300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25294. */
const char __libcgj_batch25294_marker[] = "libcgj-batch25294";

/* Product score for wave 25300 (still open / not scored). */
#define B25294_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25294_score(void)
{
	return B25294_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_25300 - report product readiness score for wave 25300.
 *
 * Always returns 0u (score still open). Soft pure-data product tag.
 * Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 0 (preserve open score).
 */
uint32_t
gj_product_score_25300(void)
{
	(void)NULL;
	return b25294_score();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_product_score_25300(void)
    __attribute__((alias("gj_product_score_25300")));
