/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6521: Deck Top 50 title rank validity gate.
 *
 * Surface (unique symbols):
 *   int gj_top50_title_rank_ok_6521(uint32_t rank);
 *     - Return 1 if rank is in [1, 50] (Deck Top 50 matrix title rank
 *       on the post-6500 title deepen path), else 0. Soft pure-data
 *       gate only.
 *   int __gj_top50_title_rank_ok_6521  (alias)
 *   __libcgj_batch6521_marker = "libcgj-batch6521"
 *
 * Exclusive continuum CREATE-ONLY (6521-6530: post-6500 top50 title
 * deepen — title_rank_ok_6521, title_rank_band_6522,
 * title_weight_6523, title_name_len_ok_6524, title_appid_ok_6525,
 * title_result_code_6526, title_stage_count_6527,
 * title_deepen_score_6528, title_threshold/ready_p_6529,
 * batch_id/milestone/continuum_6530). Unique
 * gj_top50_title_rank_ok_6521 surface only; no multi-def. Distinct
 * from gj_title_rank_ok_5721 and gj_top50_path_rank_ok_6021. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6521_marker[] = "libcgj-batch6521";

/* Deck Top 50 matrix title rank bounds (inclusive). */
#define B6521_RANK_MIN  1u
#define B6521_RANK_MAX  50u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6521_rank_ok(uint32_t uRank)
{
	if (uRank < B6521_RANK_MIN) {
		return 0;
	}
	if (uRank > B6521_RANK_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_title_rank_ok_6521 - soft check Top50 title rank is in range.
 *
 * rank: 1-based Deck Top 50 matrix rank for a title.
 * Returns 1 when rank is in [1, 50], else 0.
 * Does not consult a catalog. Does not call libc. No parent wires.
 */
int
gj_top50_title_rank_ok_6521(uint32_t uRank)
{
	(void)NULL;
	return b6521_rank_ok(uRank);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_top50_title_rank_ok_6521(uint32_t uRank)
    __attribute__((alias("gj_top50_title_rank_ok_6521")));
