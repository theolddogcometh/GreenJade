/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6207: bar3 install path deepen score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_deepen_score_6207(uint32_t abs_ok,
 *                                                   uint32_t seg_count,
 *                                                   uint32_t leaf_len);
 *     - Soft product score in [0, 100] for bar3 install-path deepen:
 *         abs_ok contributes 40 when non-zero;
 *         seg_count in [2, 16] contributes up to 40
 *           (scaled: 40 * (seg_count - 1) / 15, clamped);
 *         leaf_len in [1, 64] contributes up to 20
 *           (scaled: leaf_len > 64 ? 20 : leaf_len * 20 / 64).
 *       Inputs outside domains contribute 0 for that component.
 *   uint32_t __gj_bar3_install_path_deepen_score_6207  (alias)
 *   __libcgj_batch6207_marker = "libcgj-batch6207"
 *
 * Exclusive continuum CREATE-ONLY (6201-6210: post-6200 bar3 install
 * path deepen). Unique gj_bar3_install_path_deepen_score_6207 surface
 * only; no multi-def. Distinct from
 * gj_bar3_install_path_deepen_score_6007 (batch6007),
 * gj_product_score_clamp_5901, and gj_bar3_finalize_score_5818.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6207_marker[] = "libcgj-batch6207";

#define B6207_ABS_PTS      40u
#define B6207_DEPTH_PTS    40u
#define B6207_LEAF_PTS     20u
#define B6207_DEPTH_MIN     2u
#define B6207_DEPTH_MAX    16u
#define B6207_LEAF_CAP     64u
#define B6207_SCORE_MAX   100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6207_score(uint32_t uAbsOk, uint32_t uSegCount, uint32_t uLeafLen)
{
	uint32_t uScore;
	uint32_t uDepthPts;
	uint32_t uLeafPts;
	uint32_t uClampedSeg;
	uint32_t uClampedLeaf;

	uScore = 0u;
	if (uAbsOk != 0u) {
		uScore += B6207_ABS_PTS;
	}

	if (uSegCount >= B6207_DEPTH_MIN && uSegCount <= B6207_DEPTH_MAX) {
		/* Map [2,16] -> [~2.6, 40] via (seg-1)*40/15. */
		uClampedSeg = uSegCount - 1u;
		uDepthPts = (uClampedSeg * B6207_DEPTH_PTS) / 15u;
		if (uDepthPts > B6207_DEPTH_PTS) {
			uDepthPts = B6207_DEPTH_PTS;
		}
		uScore += uDepthPts;
	}

	if (uLeafLen >= 1u) {
		uClampedLeaf = uLeafLen;
		if (uClampedLeaf > B6207_LEAF_CAP) {
			uClampedLeaf = B6207_LEAF_CAP;
		}
		uLeafPts = (uClampedLeaf * B6207_LEAF_PTS) / B6207_LEAF_CAP;
		uScore += uLeafPts;
	}

	if (uScore > B6207_SCORE_MAX) {
		return B6207_SCORE_MAX;
	}
	return uScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_path_deepen_score_6207 - soft deepen score [0, 100].
 *
 * abs_ok:    non-zero when path is absolute with a non-empty segment
 * seg_count: non-empty path segment count
 * leaf_len:  final segment byte length
 *
 * Returns weighted soft score capped at 100. Pure integer; no libc.
 * No parent wires.
 */
uint32_t
gj_bar3_install_path_deepen_score_6207(uint32_t uAbsOk, uint32_t uSegCount,
    uint32_t uLeafLen)
{
	(void)NULL;
	return b6207_score(uAbsOk, uSegCount, uLeafLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_deepen_score_6207(uint32_t uAbsOk,
    uint32_t uSegCount, uint32_t uLeafLen)
    __attribute__((alias("gj_bar3_install_path_deepen_score_6207")));
