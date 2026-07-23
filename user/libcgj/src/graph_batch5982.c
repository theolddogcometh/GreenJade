/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5982: bar3 three-slot readiness score average.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_score_avg3_5982(uint32_t steam_score,
 *                                    uint32_t deck_score,
 *                                    uint32_t top50_score);
 *     - Floor average of three bar3 readiness component scores, each
 *       clamped to [0, 100]: (steam + deck + top50) / 3.
 *   uint32_t __gj_bar3_score_avg3_5982  (alias)
 *   __libcgj_batch5982_marker = "libcgj-batch5982"
 *
 * Exclusive continuum CREATE-ONLY (5981-5990: bar3 aggregate ready
 * score math). Unique gj_bar3_score_avg3_5982 surface only; no
 * multi-def. Distinct from gj_title_ready_score_5726 (batch5726). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5982_marker[] = "libcgj-batch5982";

#define B5982_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5982_clamp100(uint32_t uV)
{
	if (uV > B5982_SCORE_MAX) {
		return B5982_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5982_avg3(uint32_t uSteam, uint32_t uDeck, uint32_t uTop50)
{
	uint32_t uS;
	uint32_t uD;
	uint32_t uT;
	uint32_t uSum;

	uS = b5982_clamp100(uSteam);
	uD = b5982_clamp100(uDeck);
	uT = b5982_clamp100(uTop50);
	uSum = uS + uD + uT;
	return uSum / 3u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_score_avg3_5982 - floor average of three bar3 ready scores.
 *
 * steam_score / deck_score / top50_score: component scores (0..100+).
 * Returns floor((clamp(s)+clamp(d)+clamp(t)) / 3) in [0, 100].
 * Soft pure-integer aggregate math. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_score_avg3_5982(uint32_t uSteamScore, uint32_t uDeckScore,
    uint32_t uTop50Score)
{
	(void)NULL;
	return b5982_avg3(uSteamScore, uDeckScore, uTop50Score);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_score_avg3_5982(uint32_t uSteamScore,
    uint32_t uDeckScore, uint32_t uTop50Score)
    __attribute__((alias("gj_bar3_score_avg3_5982")));
