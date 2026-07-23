/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5983: bar3 three-slot readiness score sum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_score_sum3_5983(uint32_t steam_score,
 *                                    uint32_t deck_score,
 *                                    uint32_t top50_score);
 *     - Sum of three bar3 readiness component scores after clamp to
 *       [0, 100] each. Result range [0, 300].
 *   uint32_t __gj_bar3_score_sum3_5983  (alias)
 *   __libcgj_batch5983_marker = "libcgj-batch5983"
 *
 * Exclusive continuum CREATE-ONLY (5981-5990: bar3 aggregate ready
 * score math). Unique gj_bar3_score_sum3_5983 surface only; no
 * multi-def. Distinct from avg3 (batch5982). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5983_marker[] = "libcgj-batch5983";

#define B5983_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5983_clamp100(uint32_t uV)
{
	if (uV > B5983_SCORE_MAX) {
		return B5983_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5983_sum3(uint32_t uSteam, uint32_t uDeck, uint32_t uTop50)
{
	return b5983_clamp100(uSteam) + b5983_clamp100(uDeck)
	    + b5983_clamp100(uTop50);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_score_sum3_5983 - sum of three clamped bar3 ready scores.
 *
 * steam_score / deck_score / top50_score: component scores (0..100+).
 * Returns clamp(s)+clamp(d)+clamp(t) in [0, 300].
 * Soft pure-integer aggregate math. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_score_sum3_5983(uint32_t uSteamScore, uint32_t uDeckScore,
    uint32_t uTop50Score)
{
	(void)NULL;
	return b5983_sum3(uSteamScore, uDeckScore, uTop50Score);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_score_sum3_5983(uint32_t uSteamScore,
    uint32_t uDeckScore, uint32_t uTop50Score)
    __attribute__((alias("gj_bar3_score_sum3_5983")));
