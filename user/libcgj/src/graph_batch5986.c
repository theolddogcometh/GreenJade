/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5986: bar3 mask popcount readiness score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_mask_popscore_5986(uint32_t mask);
 *     - Score from low three bar3 readiness bits (steam/deck/top50):
 *       each set bit contributes 34, result clamped to [0, 100]
 *       (0, 34, 68, or 100). Soft pure-data mask→score map.
 *   uint32_t __gj_bar3_mask_popscore_5986  (alias)
 *   __libcgj_batch5986_marker = "libcgj-batch5986"
 *
 * Exclusive continuum CREATE-ONLY (5981-5990: bar3 aggregate ready
 * score math). Unique gj_bar3_mask_popscore_5986 surface only; no
 * multi-def. Distinct from gj_bar3_title_mask_score_5725 (25-step
 * max 75). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5986_marker[] = "libcgj-batch5986";

/* steam | deck | top50 (bits 0..2) */
#define B5986_BAR3_MASK    0x7u
#define B5986_SCORE_STEP   34u
#define B5986_SCORE_MAX    100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5986_pop3(uint32_t uMask)
{
	uint32_t uN;
	uint32_t uBits;

	uN = 0u;
	uBits = uMask & B5986_BAR3_MASK;
	while (uBits != 0u) {
		uN += (uBits & 1u);
		uBits >>= 1;
	}
	return uN;
}

static uint32_t
b5986_popscore(uint32_t uMask)
{
	uint32_t uScore;

	uScore = B5986_SCORE_STEP * b5986_pop3(uMask);
	if (uScore > B5986_SCORE_MAX) {
		return B5986_SCORE_MAX;
	}
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_mask_popscore_5986 - map bar3 low-three bits to a score.
 *
 * mask: product bar3 readiness bitmask (low three bits used).
 * Returns 0, 34, 68, or 100 (popcount * 34, clamped).
 * Does not probe product bars. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_mask_popscore_5986(uint32_t uMask)
{
	(void)NULL;
	return b5986_popscore(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_mask_popscore_5986(uint32_t uMask)
    __attribute__((alias("gj_bar3_mask_popscore_5986")));
