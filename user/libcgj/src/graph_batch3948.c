/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3948: product readiness score (wave 3950).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_score_3950(uint32_t mask);
 *     - Return the number of set bits among the low four product
 *       readiness slots of mask (popcount of mask & 0xf), range 0..4.
 *       Soft pure-data score for install / path / shell / libcgj
 *       product completeness on the milestone 3950 continuum.
 *   uint32_t __gj_product_score_3950  (alias)
 *   __libcgj_batch3948_marker = "libcgj-batch3948"
 *
 * Milestone 3950 exclusive continuum CREATE-ONLY (3941-3950). Unique
 * gj_product_score_3950 surface only; no multi-def. Distinct from
 * gj_product_score_3900 (batch3898), gj_product_score_3700 (batch3698),
 * gj_product_score_3600 (batch3598), and gj_product_score_u
 * (batch2998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3948_marker[] = "libcgj-batch3948";

/* Four product readiness bits (bits 0..3). */
#define B3948_PRODUCT_MASK  0xfu

/* ---- freestanding helpers ---------------------------------------------- */

/* Popcount of low four product bits (0..4). */
static uint32_t
b3948_score(uint32_t uMask)
{
	uint32_t uBits;
	uint32_t uScore;

	uBits = uMask & B3948_PRODUCT_MASK;
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
 * gj_product_score_3950 - count set product readiness slots in mask.
 *
 * mask: product readiness bitmask (bits 0..3 = path/shell/libcgj/extra)
 *
 * Returns popcount of (mask & 0xf), always in 0..4. Soft pure-data
 * only; does not walk filesystems. Does not call libc. No parent wires.
 */
uint32_t
gj_product_score_3950(uint32_t uMask)
{
	(void)NULL;
	return b3948_score(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_score_3950(uint32_t uMask)
    __attribute__((alias("gj_product_score_3950")));
