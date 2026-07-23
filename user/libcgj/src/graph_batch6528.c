/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6528: composite Top50 title deepen score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_top50_title_deepen_score_6528(uint32_t mask,
 *                                            uint32_t rank);
 *     - Aggregate stub title deepen score from stage mask and rank:
 *         stage_part = stage_count * 25  (0..4 stages → 0..100)
 *         weight     = 101-rank if rank in [1,50] else 0
 *         score      = floor((stage_part + weight) / 2) in [0, 100]
 *   uint32_t __gj_top50_title_deepen_score_6528  (alias)
 *   __libcgj_batch6528_marker = "libcgj-batch6528"
 *
 * Exclusive continuum CREATE-ONLY (6521-6530: post-6500 top50 title
 * deepen). Unique gj_top50_title_deepen_score_6528 surface only; no
 * multi-def. Distinct from gj_top50_path_score_6028 and
 * gj_bar3_deck_top50_score_5729. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6528_marker[] = "libcgj-batch6528";

#define B6528_STAGE_MASK  0xfu
#define B6528_STAGE_PTS   25u
#define B6528_RANK_MIN     1u
#define B6528_RANK_MAX    50u
#define B6528_SCORE_BASE 101u
#define B6528_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6528_stage_part(uint32_t uMask)
{
	uint32_t uBits;
	uint32_t uN;

	uBits = uMask & B6528_STAGE_MASK;
	uN = 0u;
	while (uBits != 0u) {
		uN += uBits & 1u;
		uBits >>= 1;
	}
	return uN * B6528_STAGE_PTS;
}

static uint32_t
b6528_weight(uint32_t uRank)
{
	if (uRank < B6528_RANK_MIN) {
		return 0u;
	}
	if (uRank > B6528_RANK_MAX) {
		return 0u;
	}
	return B6528_SCORE_BASE - uRank;
}

static uint32_t
b6528_score(uint32_t uMask, uint32_t uRank)
{
	uint32_t uStage;
	uint32_t uW;
	uint32_t uAvg;

	uStage = b6528_stage_part(uMask);
	uW = b6528_weight(uRank);
	uAvg = (uStage + uW) / 2u;
	if (uAvg > B6528_SCORE_MAX) {
		return B6528_SCORE_MAX;
	}
	return uAvg;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_title_deepen_score_6528 - composite Top50 title deepen score.
 *
 * mask: product Top50 title bitmask (low 4 bits = stages).
 * rank: 1-based Deck Top 50 matrix rank for the title.
 * Returns floor((stage_part + weight) / 2) in [0, 100].
 * Soft pure-data only; does not call libc. No parent wires.
 */
uint32_t
gj_top50_title_deepen_score_6528(uint32_t uMask, uint32_t uRank)
{
	(void)NULL;
	return b6528_score(uMask, uRank);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_top50_title_deepen_score_6528(uint32_t uMask, uint32_t uRank)
    __attribute__((alias("gj_top50_title_deepen_score_6528")));
