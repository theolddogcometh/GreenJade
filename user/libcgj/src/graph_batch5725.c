/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5725: bar3 title mask readiness score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_title_mask_score_5725(uint32_t mask);
 *     - Stub score from low three bar3 bits (steam/deck/top50): each
 *       set bit adds 25, base 0, max 75. Soft pure-data only.
 *   uint32_t __gj_bar3_title_mask_score_5725  (alias)
 *   __libcgj_batch5725_marker = "libcgj-batch5725"
 *
 * Exclusive continuum CREATE-ONLY (5721-5730: bar3 Deck Top 50 title
 * readiness scores). Unique gj_bar3_title_mask_score_5725 surface only;
 * no multi-def. Distinct from gj_product_bar3_bits (batch2398). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5725_marker[] = "libcgj-batch5725";

/* steam | deck | top50 (bits 0..2) */
#define B5725_BAR3_MASK   0x7u
#define B5725_SCORE_STEP  25u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5725_pop3(uint32_t uMask)
{
	uint32_t uN;
	uint32_t uBits;

	uN = 0u;
	uBits = uMask & B5725_BAR3_MASK;
	while (uBits != 0u) {
		uN += (uBits & 1u);
		uBits >>= 1;
	}
	return uN;
}

static uint32_t
b5725_mask_score(uint32_t uMask)
{
	return B5725_SCORE_STEP * b5725_pop3(uMask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_title_mask_score_5725 - stub score from bar3 title-slot bits.
 *
 * mask: product bar3 readiness bitmask (low three bits used).
 * Returns 0, 25, 50, or 75 depending on popcount of bits 0..2.
 * Does not probe product bars. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_title_mask_score_5725(uint32_t uMask)
{
	(void)NULL;
	return b5725_mask_score(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_title_mask_score_5725(uint32_t uMask)
    __attribute__((alias("gj_bar3_title_mask_score_5725")));
