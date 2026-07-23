/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6522: Deck Top 50 title rank band tier.
 *
 * Surface (unique symbols):
 *   uint32_t gj_top50_title_rank_band_6522(uint32_t rank);
 *     - Map title rank to a soft band tier:
 *         0 = invalid (outside [1, 50])
 *         1 = tier-1 (ranks 1..10)
 *         2 = tier-2 (ranks 11..25)
 *         3 = tier-3 (ranks 26..50)
 *   uint32_t __gj_top50_title_rank_band_6522  (alias)
 *   __libcgj_batch6522_marker = "libcgj-batch6522"
 *
 * Exclusive continuum CREATE-ONLY (6521-6530: post-6500 top50 title
 * deepen). Unique gj_top50_title_rank_band_6522 surface only; no
 * multi-def. Pair with gj_top50_title_rank_ok_6521. Distinct from
 * gj_top50_path_rank_band_6022. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6522_marker[] = "libcgj-batch6522";

#define B6522_RANK_MIN    1u
#define B6522_RANK_MAX   50u
#define B6522_TIER1_MAX  10u
#define B6522_TIER2_MAX  25u
#define B6522_BAND_INV    0u
#define B6522_BAND_T1     1u
#define B6522_BAND_T2     2u
#define B6522_BAND_T3     3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6522_rank_band(uint32_t uRank)
{
	if (uRank < B6522_RANK_MIN) {
		return B6522_BAND_INV;
	}
	if (uRank > B6522_RANK_MAX) {
		return B6522_BAND_INV;
	}
	if (uRank <= B6522_TIER1_MAX) {
		return B6522_BAND_T1;
	}
	if (uRank <= B6522_TIER2_MAX) {
		return B6522_BAND_T2;
	}
	return B6522_BAND_T3;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_title_rank_band_6522 - soft Top50 title rank band tier.
 *
 * rank: 1-based Deck Top 50 matrix title rank.
 * Returns 0 (invalid), 1 (1..10), 2 (11..25), or 3 (26..50).
 * Soft pure-data only; does not call libc. No parent wires.
 */
uint32_t
gj_top50_title_rank_band_6522(uint32_t uRank)
{
	(void)NULL;
	return b6522_rank_band(uRank);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_top50_title_rank_band_6522(uint32_t uRank)
    __attribute__((alias("gj_top50_title_rank_band_6522")));
