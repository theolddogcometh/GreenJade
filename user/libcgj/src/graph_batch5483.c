/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5483: Deck Top 50 rank validity predicate.
 *
 * Surface (unique symbols):
 *   int gj_deck_rank_ok_5483(uint32_t rank);
 *     - Return 1 if rank is in the closed range [1, 50] (valid Deck
 *       Top 50 rank), else 0. Soft pure-data gate only.
 *   int __gj_deck_rank_ok_5483  (alias)
 *   __libcgj_batch5483_marker = "libcgj-batch5483"
 *
 * Deck Top 50 / launcher / overlay / FPS / FSR exclusive CREATE-ONLY
 * wave (5481-5490). Unique gj_deck_rank_ok_5483 surface only; no
 * multi-def. Distinct from gj_deck_ready_score_5482. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5483_marker[] = "libcgj-batch5483";

#define B5483_RANK_MIN  1u
#define B5483_RANK_MAX  50u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5483_rank_ok(uint32_t uRank)
{
	if (uRank < B5483_RANK_MIN) {
		return 0;
	}
	if (uRank > B5483_RANK_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_rank_ok_5483 - soft check that rank is a Deck Top 50 index.
 *
 * rank: candidate 1-based title rank
 * Returns 1 when rank is in [1, 50], else 0.
 * Does not call libc. No parent wires.
 */
int
gj_deck_rank_ok_5483(uint32_t uRank)
{
	(void)NULL;
	return b5483_rank_ok(uRank);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_rank_ok_5483(uint32_t uRank)
    __attribute__((alias("gj_deck_rank_ok_5483")));
