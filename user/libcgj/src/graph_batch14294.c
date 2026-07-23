/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14294: product readiness score (wave 14300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_14300(void);
 *     - Returns 0 (product readiness score tag for the milestone 14300
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_14300  (alias)
 *   __libcgj_batch14294_marker = "libcgj-batch14294"
 *
 * Milestone 14300 exclusive continuum CREATE-ONLY (14291-14300). Unique
 * gj_product_score_14300 surface only; no multi-def. Distinct from
 * gj_product_score_14200 (batch14194), gj_product_score_14100
 * (batch14094), gj_product_score_14000 (batch13994), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14294_marker[] = "libcgj-batch14294";

/* Product readiness score tag for wave 14300 (still open). */
#define B14294_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14294_score(void)
{
	return B14294_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_14300 - report product readiness score for wave 14300.
 *
 * Always returns 0 (score still open / not scored). Soft pure-data
 * only; does not walk filesystems. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_14300(void)
{
	(void)NULL;
	return b14294_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_14300(void)
    __attribute__((alias("gj_product_score_14300")));
