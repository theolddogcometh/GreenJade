/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3398: product readiness score (wave 3400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_3400(uint32_t mask);
 *     - Return the number of set bits among the low four product
 *       readiness slots of mask (popcount of mask & 0xf), range 0..4.
 *       Soft pure-data score for install / path / shell / libcgj
 *       product completeness on the milestone 3400 continuum.
 *   uint32_t __gj_product_score_3400  (alias)
 *   __libcgj_batch3398_marker = "libcgj-batch3398"
 *
 * Milestone 3400 exclusive continuum CREATE-ONLY (3391-3400). Unique
 * gj_product_score_3400 surface only; no multi-def. Distinct from
 * gj_product_score_3300 (batch3298), gj_product_score_u (batch2998),
 * gj_product_ready_3200 (batch3199), and gj_bar3_score_3200
 * (batch3198). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3398_marker[] = "libcgj-batch3398";

/* Four product readiness bits (bits 0..3). */
#define B3398_PRODUCT_MASK  0xfu

/* ---- freestanding helpers ---------------------------------------------- */

/* Popcount of low four product bits (0..4). */
static uint32_t
b3398_score(uint32_t uMask)
{
	uint32_t uBits;
	uint32_t uScore;

	uBits = uMask & B3398_PRODUCT_MASK;
	uScore = 0u;
	if ((uBits & 0x1u) != 0u) {
		uScore = uScore + 1u;
	}
	if ((uBits & 0x2u) != 0u) {
		uScore = uScore + 1u;
	}
	if ((uBits & 0x4u) != 0u) {
		uScore = uScore + 1u;
	}
	if ((uBits & 0x8u) != 0u) {
		uScore = uScore + 1u;
	}
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_score_3400 - count set product readiness slots in mask.
 *
 * mask: product readiness bitmask (bits 0..3 = path/shell/libcgj/extra)
 *
 * Returns popcount of (mask & 0xf), always in 0..4. Soft pure-data
 * only; does not walk filesystems. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_3400(uint32_t uMask)
{
	(void)NULL;
	return b3398_score(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_3400(uint32_t uMask)
    __attribute__((alias("gj_product_score_3400")));
