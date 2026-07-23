/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5826: product top50 aggregate verify score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_top50_verify_score_5826(uint32_t steam_score,
 *                                              uint32_t deck_score,
 *                                              uint32_t top50_score);
 *     - Aggregate stub product verify score: floor((steam + deck +
 *       top50) / 3) after clamping each component to [0, 100]. Soft
 *       pure-data only.
 *   uint32_t __gj_product_top50_verify_score_5826  (alias)
 *   __libcgj_batch5826_marker = "libcgj-batch5826"
 *
 * Exclusive continuum CREATE-ONLY (5821-5830: product bar3 top50
 * verify finalize). Unique gj_product_top50_verify_score_5826 surface
 * only; no multi-def. Distinct from gj_title_ready_score_5726
 * (batch5726). Pair with component scores 5822-5824. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5826_marker[] = "libcgj-batch5826";

#define B5826_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5826_clamp100(uint32_t uV)
{
if (uV > B5826_SCORE_MAX) {
return B5826_SCORE_MAX;
}
return uV;
}

static uint32_t
b5826_verify_score(uint32_t uSteamScore, uint32_t uDeckScore,
    uint32_t uTop50Score)
{
uint32_t uS;
uint32_t uD;
uint32_t uT;
uint32_t uSum;

uS = b5826_clamp100(uSteamScore);
uD = b5826_clamp100(uDeckScore);
uT = b5826_clamp100(uTop50Score);
uSum = uS + uD + uT;
return uSum / 3u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_top50_verify_score_5826 - aggregate stub product verify score.
 *
 * steam_score: Steam slot component (typically from 5822).
 * deck_score:  Deck slot component (typically from 5823).
 * top50_score: Top50 rank component (typically from 5824).
 * Returns floor((steam + deck + top50) / 3) after clamp to [0, 100].
 * Does not walk catalogs. Does not call libc. No parent wires.
 */
uint32_t
gj_product_top50_verify_score_5826(uint32_t uSteamScore, uint32_t uDeckScore,
    uint32_t uTop50Score)
{
(void)NULL;
return b5826_verify_score(uSteamScore, uDeckScore, uTop50Score);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_top50_verify_score_5826(uint32_t uSteamScore,
    uint32_t uDeckScore, uint32_t uTop50Score)
    __attribute__((alias("gj_product_top50_verify_score_5826")));
