/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8994: product readiness score (wave 9000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_9000(void);
 *     - Returns the product readiness score tag for the milestone 9000
 *       continuum (always 9000). Soft compile-time product score tag.
 *   uint32_t __gj_product_score_9000  (alias)
 *   __libcgj_batch8994_marker = "libcgj-batch8994"
 *
 * Milestone 9000 exclusive continuum CREATE-ONLY (8991-9000). Unique
 * gj_product_score_9000 surface only; no multi-def. Distinct from
 * gj_product_score_8900 (batch8894), gj_product_score_8800 (batch8794),
 * gj_product_score_8700 (batch8694), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8994_marker[] = "libcgj-batch8994";

/* Product readiness score tag for wave 9000. */
#define B8994_PRODUCT_SCORE  9000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8994_score(void)
{
	return B8994_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_9000 - report product readiness score for wave 9000.
 *
 * Always returns 9000 (wave-tagged soft product score integer). Soft
 * pure-data only; does not walk filesystems. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_product_score_9000(void)
{
	(void)NULL;
	return b8994_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_9000(void)
    __attribute__((alias("gj_product_score_9000")));
