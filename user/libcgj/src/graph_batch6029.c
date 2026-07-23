/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6029: Top50 path ready threshold + gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_top50_path_threshold_6029(void);
 *     - Returns the soft path-ready threshold score (70) for post-6000
 *       Top50 path deepen. Compile-time product tag only.
 *   int gj_top50_path_ready_p_6029(uint32_t score);
 *     - Return 1 if score is in [70, 100] (soft path-ready band), else
 *       0. Pure-data lamp only.
 *   uint32_t __gj_top50_path_threshold_6029  (alias)
 *   int __gj_top50_path_ready_p_6029  (alias)
 *   __libcgj_batch6029_marker = "libcgj-batch6029"
 *
 * Exclusive continuum CREATE-ONLY (6021-6030: post-6000 top50 path
 * deepen). Unique surfaces only; no multi-def. Distinct from
 * gj_title_ready_threshold_5727 / gj_title_ready_p_5728. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6029_marker[] = "libcgj-batch6029";

#define B6029_THRESHOLD  70u
#define B6029_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6029_threshold(void)
{
	return B6029_THRESHOLD;
}

static int
b6029_ready_p(uint32_t uScore)
{
	if (uScore < B6029_THRESHOLD) {
		return 0;
	}
	if (uScore > B6029_SCORE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_path_threshold_6029 - report Top50 path-ready score threshold.
 *
 * Always returns 70. Soft product tag only. No parent wires.
 */
uint32_t
gj_top50_path_threshold_6029(void)
{
	(void)NULL;
	return b6029_threshold();
}

/*
 * gj_top50_path_ready_p_6029 - soft Top50 path-ready lamp for score.
 *
 * score: composite path deepen score (typically from 6028).
 * Returns 1 if score is in [70, 100], else 0.
 * Soft pure-data only; does not call libc. No parent wires.
 */
int
gj_top50_path_ready_p_6029(uint32_t uScore)
{
	return b6029_ready_p(uScore);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_top50_path_threshold_6029(void)
    __attribute__((alias("gj_top50_path_threshold_6029")));

int __gj_top50_path_ready_p_6029(uint32_t uScore)
    __attribute__((alias("gj_top50_path_ready_p_6029")));
