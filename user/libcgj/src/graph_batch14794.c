/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14794: product readiness score tag (wave 14800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_14800(void);
 *     - Returns 0 (product readiness score tag for the milestone 14800
 *       continuum). Soft compile-time product score tag; still open.
 *   uint32_t __gj_product_score_14800  (alias)
 *   __libcgj_batch14794_marker = "libcgj-batch14794"
 *
 * Milestone 14800 exclusive continuum CREATE-ONLY (14791-14800). Unique surface
 * only; no multi-def. Distinct from gj_*_14700 / gj_*_14600 / gj_*_14500 milestone
 * surfaces and sibling 14800 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14794_marker[] = "libcgj-batch14794";

/* Product score for wave 14800 (still open). */
#define B14794_PRODUCT_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14794_score(void)
{
	return B14794_PRODUCT_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_14800 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_product_score_14800(void)
{
	(void)NULL;
	return b14794_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_14800(void)
    __attribute__((alias("gj_product_score_14800")));
