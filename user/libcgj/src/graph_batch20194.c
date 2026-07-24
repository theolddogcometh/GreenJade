/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20194: product readiness score tag (wave 20200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_20200(void);
 *     - Returns 0 (product readiness score tag for the milestone 20200
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_20200  (alias)
 *   __libcgj_batch20194_marker = "libcgj-batch20194"
 *
 * Milestone 20200 exclusive continuum CREATE-ONLY (20191-20200). Unique
 * gj_product_score_20200 surface only; no multi-def. Distinct from
 * gj_product_score_20100 / gj_product_score_20000 / gj_product_score_19900
 * and sibling 20200 milestone symbols.
 * No parent wires. No __int128.
 *
 * HONESTY: product_score remains intentionally 0. Soft continuum graph
 * growth is not a product readiness claim; score stays open until a
 * real product-score track advances it.
 *
 * CGJ soft marker band (20191–20200): host soft probes
 * (cgj_soft_milestone_20200.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20194. */
const char __libcgj_batch20194_marker[] = "libcgj-batch20194";

/* Product score for wave 20200 (still open / not scored). */
#define B20194_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20194_score(void)
{
	return B20194_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_20200 - report product readiness score for wave 20200.
 *
 * Always returns 0u (score still open). Soft pure-data product tag.
 * Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 0 (preserve open score).
 */
uint32_t
gj_product_score_20200(void)
{
	(void)NULL;
	return b20194_score();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_product_score_20200(void)
    __attribute__((alias("gj_product_score_20200")));
