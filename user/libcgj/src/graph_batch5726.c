/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5726: composite title readiness score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_title_ready_score_5726(uint32_t steam_score,
 *                                      uint32_t deck_score,
 *                                      uint32_t top50_score);
 *     - Aggregate stub title readiness score: average of three
 *       component scores each clamped to [0, 100], rounded down.
 *   uint32_t __gj_title_ready_score_5726  (alias)
 *   __libcgj_batch5726_marker = "libcgj-batch5726"
 *
 * Exclusive continuum CREATE-ONLY (5721-5730: bar3 Deck Top 50 title
 * readiness scores). Unique gj_title_ready_score_5726 surface only; no
 * multi-def. Pair with 5722/5723/5724 component scores. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5726_marker[] = "libcgj-batch5726";

#define B5726_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5726_clamp100(uint32_t uV)
{
	if (uV > B5726_SCORE_MAX) {
		return B5726_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5726_ready_score(uint32_t uSteam, uint32_t uDeck, uint32_t uTop50)
{
	uint32_t uS;
	uint32_t uD;
	uint32_t uT;
	uint32_t uSum;

	uS = b5726_clamp100(uSteam);
	uD = b5726_clamp100(uDeck);
	uT = b5726_clamp100(uTop50);
	uSum = uS + uD + uT;
	return uSum / 3u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_title_ready_score_5726 - aggregate stub title readiness score.
 *
 * steam_score: Steam slot component (typically from 5722).
 * deck_score:  Deck slot component (typically from 5723).
 * top50_score: Top50 rank component (typically from 5724).
 * Returns floor((steam + deck + top50) / 3) after clamp to [0, 100].
 * Does not walk catalogs. Does not call libc. No parent wires.
 */
uint32_t
gj_title_ready_score_5726(uint32_t uSteamScore, uint32_t uDeckScore,
    uint32_t uTop50Score)
{
	(void)NULL;
	return b5726_ready_score(uSteamScore, uDeckScore, uTop50Score);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_title_ready_score_5726(uint32_t uSteamScore,
    uint32_t uDeckScore, uint32_t uTop50Score)
    __attribute__((alias("gj_title_ready_score_5726")));
