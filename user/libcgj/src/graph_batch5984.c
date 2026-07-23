/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5984: bar3 three-slot readiness score minimum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_score_min3_5984(uint32_t steam_score,
 *                                    uint32_t deck_score,
 *                                    uint32_t top50_score);
 *     - Minimum of three bar3 readiness component scores after each is
 *       clamped to [0, 100]. Soft bottleneck / weakest-slot math.
 *   uint32_t __gj_bar3_score_min3_5984  (alias)
 *   __libcgj_batch5984_marker = "libcgj-batch5984"
 *
 * Exclusive continuum CREATE-ONLY (5981-5990: bar3 aggregate ready
 * score math). Unique gj_bar3_score_min3_5984 surface only; no
 * multi-def. Distinct from max3 (batch5985). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5984_marker[] = "libcgj-batch5984";

#define B5984_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5984_clamp100(uint32_t uV)
{
	if (uV > B5984_SCORE_MAX) {
		return B5984_SCORE_MAX;
	}
	return uV;
}

static uint32_t
b5984_min2(uint32_t uA, uint32_t uB)
{
	if (uA < uB) {
		return uA;
	}
	return uB;
}

static uint32_t
b5984_min3(uint32_t uSteam, uint32_t uDeck, uint32_t uTop50)
{
	uint32_t uS;
	uint32_t uD;
	uint32_t uT;

	uS = b5984_clamp100(uSteam);
	uD = b5984_clamp100(uDeck);
	uT = b5984_clamp100(uTop50);
	return b5984_min2(uS, b5984_min2(uD, uT));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_score_min3_5984 - minimum of three clamped bar3 ready scores.
 *
 * steam_score / deck_score / top50_score: component scores (0..100+).
 * Returns min(clamp(s), clamp(d), clamp(t)) in [0, 100].
 * Soft pure-integer aggregate math. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_score_min3_5984(uint32_t uSteamScore, uint32_t uDeckScore,
    uint32_t uTop50Score)
{
	(void)NULL;
	return b5984_min3(uSteamScore, uDeckScore, uTop50Score);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_score_min3_5984(uint32_t uSteamScore,
    uint32_t uDeckScore, uint32_t uTop50Score)
    __attribute__((alias("gj_bar3_score_min3_5984")));
