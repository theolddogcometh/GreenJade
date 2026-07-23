/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13994: product readiness score (wave 14000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_14000(void);
 *     - Returns 0 (product readiness score tag for the milestone 14000
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_14000  (alias)
 *   __libcgj_batch13994_marker = "libcgj-batch13994"
 *
 * Milestone 14000 exclusive continuum CREATE-ONLY (13991-14000). Unique
 * gj_product_score_14000 surface only; no multi-def. Distinct from
 * gj_product_score_13000 (batch12994), gj_product_score_12900
 * (batch12894), gj_product_score_12800 (batch12794), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13994_marker[] = "libcgj-batch13994";

/* Product readiness score tag for wave 14000 (still open). */
#define B13994_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13994_score(void)
{
	return B13994_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_14000 - report product readiness score for wave 14000.
 *
 * Always returns 0 (score still open / not scored). Soft pure-data
 * only; does not walk filesystems. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_14000(void)
{
	(void)NULL;
	return b13994_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_14000(void)
    __attribute__((alias("gj_product_score_14000")));
