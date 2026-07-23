/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6364: cloud save path depth-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cloud_save_path_depth_ok_6364(uint32_t seg_count);
 *     - Returns 1 if seg_count is in [2, 16], else 0. Soft depth gate
 *       for cloud save object keys (tenant/user/.../slot).
 *   uint32_t __gj_cloud_save_path_depth_ok_6364  (alias)
 *   __libcgj_batch6364_marker = "libcgj-batch6364"
 *
 * Exclusive continuum CREATE-ONLY (6361-6370: cloud save path stubs —
 * max_len_6361, abs_ok_6362, seg_count_6363, depth_ok_6364,
 * len_ok_6365, leaf_len_6366, leaf_ok_6367, score_6368, pack_6369,
 * batch_id / wave_ready_6370). Unique surface only; no multi-def.
 * Distinct from gj_bar3_install_path_depth_ok_6205. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6364_marker[] = "libcgj-batch6364";

#define B6364_DEPTH_MIN  2u
#define B6364_DEPTH_MAX  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6364_depth_ok(uint32_t uSegCount)
{
	if (uSegCount < B6364_DEPTH_MIN) {
		return 0u;
	}
	if (uSegCount > B6364_DEPTH_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cloud_save_path_depth_ok_6364 - segment depth in [2, 16] gate.
 *
 * seg_count: number of non-empty path segments
 *
 * Returns 1 when depth is in range, else 0. Soft pure-data only.
 * No parent wires.
 */
uint32_t
gj_cloud_save_path_depth_ok_6364(uint32_t uSegCount)
{
	(void)NULL;
	return b6364_depth_ok(uSegCount);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cloud_save_path_depth_ok_6364(uint32_t uSegCount)
    __attribute__((alias("gj_cloud_save_path_depth_ok_6364")));
