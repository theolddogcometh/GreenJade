/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5989: bar3 weighted aggregate ready score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_aggregate_ready_score_5989(uint32_t steam_score,
 *                                               uint32_t deck_score,
 *                                               uint32_t top50_score);
 *     - Weighted aggregate readiness score after clamp to [0, 100]:
 *       floor((2*steam + 2*deck + top50) / 5). Emphasizes steam/deck
 *       product slots over top50 rank weight. Result in [0, 100].
 *   uint32_t __gj_bar3_aggregate_ready_score_5989  (alias)
 *   __libcgj_batch5989_marker = "libcgj-batch5989"
 *
 * Exclusive continuum CREATE-ONLY (5981-5990: bar3 aggregate ready
 * score math). Unique gj_bar3_aggregate_ready_score_5989 surface only;
 * no multi-def. Distinct from equal avg3 (batch5982) and
 * gj_title_ready_score_5726 (batch5726). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5989_marker[] = "libcgj-batch5989";

#define B5989_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5989_clamp100(uint32_t uV)
{
	if (uV > B5989_SCORE_MAX) {
		return B5989_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5989_aggregate(uint32_t uSteam, uint32_t uDeck, uint32_t uTop50)
{
	uint32_t uS;
	uint32_t uD;
	uint32_t uT;
	uint32_t uW;

	uS = b5989_clamp100(uSteam);
	uD = b5989_clamp100(uDeck);
	uT = b5989_clamp100(uTop50);
	/* 2*steam + 2*deck + top50  (weights sum to 5) */
	uW = (uS << 1) + (uD << 1) + uT;
	return uW / 5u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_aggregate_ready_score_5989 - weighted bar3 aggregate ready score.
 *
 * steam_score / deck_score / top50_score: component scores (0..100+).
 * Returns floor((2*clamp(s) + 2*clamp(d) + clamp(t)) / 5) in [0, 100].
 * Soft pure-integer aggregate math. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_aggregate_ready_score_5989(uint32_t uSteamScore,
    uint32_t uDeckScore, uint32_t uTop50Score)
{
	(void)NULL;
	return b5989_aggregate(uSteamScore, uDeckScore, uTop50Score);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_aggregate_ready_score_5989(uint32_t uSteamScore,
    uint32_t uDeckScore, uint32_t uTop50Score)
    __attribute__((alias("gj_bar3_aggregate_ready_score_5989")));
