/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2635: Deck Top 50 rank soft-hint predicate.
 *
 * Surface (unique symbols):
 *   int gj_deck_top50_hint_p(unsigned rank);
 *     - Return 1 if rank is a Deck Top 50 matrix rank in the closed
 *       product range 1..50, else 0. Soft pure-data gate for the
 *       adoption matrix (matrix/deck-top50-*.md); not a catalog probe.
 *   int __gj_deck_top50_hint_p  (alias)
 *   __libcgj_batch2635_marker = "libcgj-batch2635"
 *
 * Product path exclusive wave (2631-2640). Distinct from
 * gj_product_bar_top50_hint (batch1993 always-1 capability bit) and
 * gj_deck_model_ok / gj_deck_res_is_native_hint — unique
 * gj_deck_top50_hint_p surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2635_marker[] = "libcgj-batch2635";

/* Inclusive Deck Top 50 matrix rank band. */
#define B2635_RANK_MIN  1u
#define B2635_RANK_MAX  50u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if rank is in [1, 50]. */
static int
b2635_rank_ok(unsigned uRank)
{
	if (uRank < B2635_RANK_MIN) {
		return 0;
	}
	if (uRank > B2635_RANK_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_top50_hint_p - soft check for a Deck Top 50 matrix rank.
 *
 * rank: 1-based matrix rank (1 = top title, 50 = last of the set)
 *
 * Returns 1 when 1 <= rank <= 50, else 0. Does not call libc or
 * consult the matrix document.
 */
int
gj_deck_top50_hint_p(unsigned uRank)
{
	(void)NULL;
	return b2635_rank_ok(uRank);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_top50_hint_p(unsigned uRank)
    __attribute__((alias("gj_deck_top50_hint_p")));
