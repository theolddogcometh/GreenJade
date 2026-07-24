/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87294: product readiness score tag (wave 87300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_87300(void);
 *     - Returns 0 (product readiness score tag for the milestone 87300
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_87300  (alias)
 *   __libcgj_batch87294_marker = "libcgj-batch87294"
 *
 * Milestone 87300 exclusive continuum CREATE-ONLY (25891-87300). Unique
 * gj_product_score_87300 surface only; no multi-def. Distinct from
 * gj_product_score_25800 / gj_product_score_25700 / gj_product_score_25600
 * and sibling 87300 milestone symbols.
 * No parent wires. No __int128.
 *
 * HONESTY: product_score remains intentionally 0. Soft continuum graph
 * growth is not a product readiness claim; score stays open until a
 * real product-score track advances it.
 *
 * CGJ soft marker band (25891–87300): host soft probes
 * (cgj_soft_milestone_87300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=87300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 87294. */
const char __libcgj_batch87294_marker[] = "libcgj-batch87294";

/* Product score for wave 87300 (still open / not scored). */
#define B87294_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87294_score(void)
{
	return B87294_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_87300 - report product readiness score for wave 87300.
 *
 * Always returns 0u (score still open). Soft pure-data product tag.
 * Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 0 (preserve open score).
 */
uint32_t
gj_product_score_87300(void)
{
	(void)NULL;
	return b87294_score();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_product_score_87300(void)
    __attribute__((alias("gj_product_score_87300")));
