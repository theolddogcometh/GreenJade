/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6523: Deck Top 50 title rank weight score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_top50_title_weight_6523(uint32_t rank);
 *     - Stub title weight score in [0, 100]. rank outside [1, 50] → 0;
 *       else score = 101 - rank (rank 1 → 100, rank 50 → 51). Soft
 *       pure-data matrix weight for post-6500 title deepen.
 *   uint32_t __gj_top50_title_weight_6523  (alias)
 *   __libcgj_batch6523_marker = "libcgj-batch6523"
 *
 * Exclusive continuum CREATE-ONLY (6521-6530: post-6500 top50 title
 * deepen). Unique gj_top50_title_weight_6523 surface only; no multi-def.
 * Distinct from gj_top50_path_weight_6023 and gj_title_top50_score_5724.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6523_marker[] = "libcgj-batch6523";

#define B6523_RANK_MIN   1u
#define B6523_RANK_MAX  50u
#define B6523_SCORE_BASE 101u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6523_weight(uint32_t uRank)
{
	if (uRank < B6523_RANK_MIN) {
		return 0u;
	}
	if (uRank > B6523_RANK_MAX) {
		return 0u;
	}
	return B6523_SCORE_BASE - uRank;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_title_weight_6523 - stub Top50 title rank weight score.
 *
 * rank: 1-based Deck Top 50 matrix rank for a title.
 * Returns 0 if rank not in [1, 50]; else (101 - rank) in [51, 100].
 * Does not read a catalog. Does not call libc. No parent wires.
 */
uint32_t
gj_top50_title_weight_6523(uint32_t uRank)
{
	(void)NULL;
	return b6523_weight(uRank);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_top50_title_weight_6523(uint32_t uRank)
    __attribute__((alias("gj_top50_title_weight_6523")));
