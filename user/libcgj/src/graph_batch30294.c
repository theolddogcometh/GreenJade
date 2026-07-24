/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30294: product readiness score tag (wave 30300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_30300(void);
 *     - Returns 0 (product readiness score tag for the milestone 30300
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_30300  (alias)
 *   __libcgj_batch30294_marker = "libcgj-batch30294"
 *
 * Milestone 30300 exclusive continuum CREATE-ONLY (30291-30300). Unique
 * gj_product_score_30300 surface only; no multi-def. Distinct from
 * gj_product_score_30200 / gj_product_score_30100 / gj_product_score_30000
 * and sibling 30300 milestone symbols.
 * No parent wires. No __int128.
 *
 * HONESTY: product_score remains intentionally 0. Soft continuum graph
 * growth is not a product readiness claim; score stays open until a
 * real product-score track advances it.
 *
 * CGJ soft marker band (30291–30300): host soft probes
 * (cgj_soft_milestone_30300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30294. */
const char __libcgj_batch30294_marker[] = "libcgj-batch30294";

/* Product score for wave 30300 (still open / not scored). */
#define B30294_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30294_score(void)
{
	return B30294_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_30300 - report product readiness score for wave 30300.
 *
 * Always returns 0u (score still open). Soft pure-data product tag.
 * Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 0 (preserve open score).
 */
uint32_t
gj_product_score_30300(void)
{
	(void)NULL;
	return b30294_score();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_product_score_30300(void)
    __attribute__((alias("gj_product_score_30300")));
