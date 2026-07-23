/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5981: bar3 readiness score clamp [0, 100].
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_score_clamp_5981(uint32_t score);
 *     - Clamp a soft bar3 readiness score into [0, 100]. Values above
 *       100 become 100; score is unsigned so the floor is always 0.
 *   uint32_t __gj_bar3_score_clamp_5981  (alias)
 *   __libcgj_batch5981_marker = "libcgj-batch5981"
 *
 * Exclusive continuum CREATE-ONLY (5981-5990: bar3 aggregate ready
 * score math — score_clamp, score_avg3, score_sum3, score_min3,
 * score_max3, mask_popscore, ready_threshold, ready_score_p,
 * aggregate_ready_score, continuum + batch_id_5990). Unique
 * gj_bar3_score_clamp_5981 surface only; no multi-def. Distinct from
 * title-score clamps in 5726. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5981_marker[] = "libcgj-batch5981";

/* Soft bar3 readiness score scale (0..100). */
#define B5981_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5981_score_clamp(uint32_t uScore)
{
	if (uScore > B5981_SCORE_MAX) {
		return B5981_SCORE_MAX;
	}
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_score_clamp_5981 - clamp bar3 readiness score to [0, 100].
 *
 * score: raw soft readiness component or aggregate.
 * Returns score when score <= 100, else 100.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_score_clamp_5981(uint32_t uScore)
{
	(void)NULL;
	return b5981_score_clamp(uScore);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_score_clamp_5981(uint32_t uScore)
    __attribute__((alias("gj_bar3_score_clamp_5981")));
