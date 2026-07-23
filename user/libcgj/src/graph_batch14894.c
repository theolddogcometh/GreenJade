/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14894: product readiness score tag (wave 14900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_14900(void);
 *     - Returns 0 (product readiness score tag for the milestone 14900
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_14900  (alias)
 *   __libcgj_batch14894_marker = "libcgj-batch14894"
 *
 * Milestone 14900 exclusive continuum CREATE-ONLY (14891-14900). Unique surface
 * only; no multi-def. Distinct from gj_*_14800 / gj_*_14700 / gj_*_14600 milestone
 * surfaces and sibling 14900 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14894_marker[] = "libcgj-batch14894";

/* Product score for wave 14900 (still open). */
#define B14894_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14894_score(void)
{
	return B14894_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_14900 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_product_score_14900(void)
{
	(void)NULL;
	return b14894_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_14900(void)
    __attribute__((alias("gj_product_score_14900")));
