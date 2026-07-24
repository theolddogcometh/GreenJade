/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20994: product readiness score tag (wave 21000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_21000(void);
 *     - Returns 0 (product readiness score tag for the milestone 21000
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_21000  (alias)
 *   __libcgj_batch20994_marker = "libcgj-batch20994"
 *
 * Milestone 21000 exclusive continuum CREATE-ONLY (20991-21000). Unique
 * gj_product_score_21000 surface only; no multi-def. Distinct from
 * gj_product_score_20900 / gj_product_score_20800 / gj_product_score_20700
 * and sibling 21000 milestone symbols.
 * No parent wires. No __int128.
 *
 * HONESTY: product_score remains intentionally 0. Soft continuum graph
 * growth is not a product readiness claim; score stays open until a
 * real product-score track advances it.
 *
 * CGJ soft marker band (20991–21000): host soft probes
 * (cgj_soft_milestone_21000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20994. */
const char __libcgj_batch20994_marker[] = "libcgj-batch20994";

/* Product score for wave 21000 (still open / not scored). */
#define B20994_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20994_score(void)
{
	return B20994_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_21000 - report product readiness score for wave 21000.
 *
 * Always returns 0u (score still open). Soft pure-data product tag.
 * Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 0 (preserve open score).
 */
uint32_t
gj_product_score_21000(void)
{
	(void)NULL;
	return b20994_score();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_product_score_21000(void)
    __attribute__((alias("gj_product_score_21000")));
