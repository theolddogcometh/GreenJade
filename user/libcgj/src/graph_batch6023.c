/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6023: Deck Top 50 path rank weight score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_top50_path_weight_6023(uint32_t rank);
 *     - Stub path weight score in [0, 100]. rank outside [1, 50] → 0;
 *       else score = 101 - rank (rank 1 → 100, rank 50 → 51). Soft
 *       pure-data matrix weight for post-6000 path deepen.
 *   uint32_t __gj_top50_path_weight_6023  (alias)
 *   __libcgj_batch6023_marker = "libcgj-batch6023"
 *
 * Exclusive continuum CREATE-ONLY (6021-6030: post-6000 top50 path
 * deepen). Unique gj_top50_path_weight_6023 surface only; no multi-def.
 * Distinct from gj_title_top50_score_5724 and gj_deck_ready_score_5482.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6023_marker[] = "libcgj-batch6023";

#define B6023_RANK_MIN   1u
#define B6023_RANK_MAX   50u
#define B6023_SCORE_BASE 101u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6023_weight(uint32_t uRank)
{
	if (uRank < B6023_RANK_MIN) {
		return 0u;
	}
	if (uRank > B6023_RANK_MAX) {
		return 0u;
	}
	return B6023_SCORE_BASE - uRank;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_path_weight_6023 - stub Top50 path rank weight score.
 *
 * rank: 1-based Deck Top 50 matrix rank for a title.
 * Returns 0 if rank not in [1, 50]; else (101 - rank) in [51, 100].
 * Does not read a catalog. Does not call libc. No parent wires.
 */
uint32_t
gj_top50_path_weight_6023(uint32_t uRank)
{
	(void)NULL;
	return b6023_weight(uRank);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_top50_path_weight_6023(uint32_t uRank)
    __attribute__((alias("gj_top50_path_weight_6023")));
