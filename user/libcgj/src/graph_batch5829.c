/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5829: bar3 top50 finalize score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_top50_finalize_score_5829(uint32_t mask,
 *                                             uint32_t rank);
 *     - Aggregate stub finalize score from product bar3 mask and Deck
 *       Top 50 rank. Combines steam/deck bit lamps with rank weight:
 *       floor((steam + deck + top50) / 3) on [0, 100] scale. Invalid
 *       rank → top50 component 0.
 *   uint32_t __gj_bar3_top50_finalize_score_5829  (alias)
 *   __libcgj_batch5829_marker = "libcgj-batch5829"
 *
 * Exclusive continuum CREATE-ONLY (5821-5830: product bar3 top50
 * verify finalize). Unique gj_bar3_top50_finalize_score_5829 surface
 * only; no multi-def. Distinct from component scores 5822-5826 and
 * gj_bar3_deck_top50_score_5729 (batch5729). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5829_marker[] = "libcgj-batch5829";

#define B5829_STEAM_BIT   0x1u
#define B5829_DECK_BIT    0x2u
#define B5829_RANK_MIN    1u
#define B5829_RANK_MAX    50u
#define B5829_SCORE_FULL  100u
#define B5829_SCORE_BASE  101u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5829_steam(uint32_t uMask)
{
if ((uMask & B5829_STEAM_BIT) != 0u) {
return B5829_SCORE_FULL;
}
return 0u;
}

static uint32_t
b5829_deck(uint32_t uMask)
{
if ((uMask & B5829_DECK_BIT) != 0u) {
return B5829_SCORE_FULL;
}
return 0u;
}

static uint32_t
b5829_top50(uint32_t uRank)
{
if (uRank < B5829_RANK_MIN) {
return 0u;
}
if (uRank > B5829_RANK_MAX) {
return 0u;
}
return B5829_SCORE_BASE - uRank;
}

static uint32_t
b5829_score(uint32_t uMask, uint32_t uRank)
{
uint32_t uS;
uint32_t uD;
uint32_t uT;

uS = b5829_steam(uMask);
uD = b5829_deck(uMask);
uT = b5829_top50(uRank);
return (uS + uD + uT) / 3u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_top50_finalize_score_5829 - full bar3 Top50 finalize score.
 *
 * mask: product bar3 readiness bitmask (bit0 Steam, bit1 Deck).
 * rank: 1-based Deck Top 50 matrix rank for the title.
 * Returns floor((steam + deck + top50) / 3) in [0, 100].
 * Soft pure-data only; does not call libc. No parent wires.
 */
uint32_t
gj_bar3_top50_finalize_score_5829(uint32_t uMask, uint32_t uRank)
{
(void)NULL;
return b5829_score(uMask, uRank);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_top50_finalize_score_5829(uint32_t uMask, uint32_t uRank)
    __attribute__((alias("gj_bar3_top50_finalize_score_5829")));
