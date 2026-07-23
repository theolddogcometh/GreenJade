/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5721: Deck Top 50 title rank gate.
 *
 * Surface (unique symbols):
 *   int gj_title_rank_ok_5721(uint32_t rank);
 *     - Return 1 if rank is in [1, 50] (Deck Top 50 matrix rank),
 *       else 0. Soft pure-data gate only.
 *   int __gj_title_rank_ok_5721  (alias)
 *   __libcgj_batch5721_marker = "libcgj-batch5721"
 *
 * Exclusive continuum CREATE-ONLY (5721-5730: bar3 Deck Top 50 title
 * readiness scores). Unique gj_title_rank_ok_5721 surface only; no
 * multi-def. Distinct from gj_deck_top50_hint_p (batch2635). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5721_marker[] = "libcgj-batch5721";

/* Deck Top 50 matrix rank bounds (inclusive). */
#define B5721_RANK_MIN  1u
#define B5721_RANK_MAX  50u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5721_rank_ok(uint32_t uRank)
{
	if (uRank < B5721_RANK_MIN) {
		return 0;
	}
	if (uRank > B5721_RANK_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_title_rank_ok_5721 - soft check Deck Top 50 title rank is in range.
 *
 * rank: 1-based Deck Top 50 matrix rank for a title.
 * Returns 1 when rank is in [1, 50], else 0.
 * Does not consult a catalog. Does not call libc. No parent wires.
 */
int
gj_title_rank_ok_5721(uint32_t uRank)
{
	(void)NULL;
	return b5721_rank_ok(uRank);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_title_rank_ok_5721(uint32_t uRank)
    __attribute__((alias("gj_title_rank_ok_5721")));
