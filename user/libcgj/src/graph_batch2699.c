/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2699: product bar3 readiness score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_score_u(uint32_t mask);
 *     - Return the number of set bits among the low three product bar3
 *       slots of mask (popcount of mask & 0x7), range 0..3. Soft
 *       pure-data score for Steam/Deck/Top50 completeness.
 *   uint32_t __gj_bar3_score_u  (alias)
 *   __libcgj_batch2699_marker = "libcgj-batch2699"
 *
 * Milestone 2700 exclusive product helpers (2691-2700). Unique
 * gj_bar3_score_u surface only; no multi-def. Distinct from
 * gj_product_bar3_mask (batch2597 constant 0x7), gj_product_bar3_bits
 * (batch2398 extract), gj_bar3_steam_bit (batch2498), gj_bar3_deck_bit
 * (batch2499), and gj_bar3_ready_hint (batch2299).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2699_marker[] = "libcgj-batch2699";

/* Three product bar3 readiness bits (bits 0..2). */
#define B2699_BAR3_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

/* Popcount of low three bar3 bits (0..3). */
static uint32_t
b2699_score(uint32_t uMask)
{
	uint32_t uBits;
	uint32_t uScore;

	uBits = uMask & B2699_BAR3_MASK;
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
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_score_u - count set bar3 readiness slots in mask.
 *
 * mask: product bar3 readiness bitmask (bits 0..2 = Steam/Deck/Top50)
 *
 * Returns popcount of (mask & 0x7), always in 0..3. Soft pure-data
 * only; does not probe Steam/Deck. Does not call libc.
 */
uint32_t
gj_bar3_score_u(uint32_t uMask)
{
	(void)NULL;
	return b2699_score(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_score_u(uint32_t uMask)
    __attribute__((alias("gj_bar3_score_u")));
