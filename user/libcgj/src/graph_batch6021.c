/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6021: Deck Top 50 path rank validity gate.
 *
 * Surface (unique symbols):
 *   int gj_top50_path_rank_ok_6021(uint32_t rank);
 *     - Return 1 if rank is in [1, 50] (Deck Top 50 matrix rank on the
 *       post-6000 product path), else 0. Soft pure-data gate only.
 *   int __gj_top50_path_rank_ok_6021  (alias)
 *   __libcgj_batch6021_marker = "libcgj-batch6021"
 *
 * Exclusive continuum CREATE-ONLY (6021-6030: post-6000 top50 path
 * deepen). Unique gj_top50_path_rank_ok_6021 surface only; no multi-def.
 * Distinct from gj_title_rank_ok_5721 and gj_deck_rank_ok_5483. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6021_marker[] = "libcgj-batch6021";

/* Deck Top 50 matrix rank bounds (inclusive). */
#define B6021_RANK_MIN  1u
#define B6021_RANK_MAX  50u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6021_rank_ok(uint32_t uRank)
{
	if (uRank < B6021_RANK_MIN) {
		return 0;
	}
	if (uRank > B6021_RANK_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_path_rank_ok_6021 - soft check Top50 path rank is in range.
 *
 * rank: 1-based Deck Top 50 matrix rank for a title on the product path.
 * Returns 1 when rank is in [1, 50], else 0.
 * Does not consult a catalog. Does not call libc. No parent wires.
 */
int
gj_top50_path_rank_ok_6021(uint32_t uRank)
{
	(void)NULL;
	return b6021_rank_ok(uRank);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_top50_path_rank_ok_6021(uint32_t uRank)
    __attribute__((alias("gj_top50_path_rank_ok_6021")));
