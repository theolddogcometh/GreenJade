/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6367: cloud save path leaf-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cloud_save_path_leaf_ok_6367(uint32_t leaf_len);
 *     - Returns 1 if leaf_len is in [1, 255], else 0. Soft leaf name
 *       length gate for cloud save object basenames.
 *   uint32_t __gj_cloud_save_path_leaf_ok_6367  (alias)
 *   __libcgj_batch6367_marker = "libcgj-batch6367"
 *
 * Exclusive continuum CREATE-ONLY (6361-6370: cloud save path stubs —
 * max_len_6361, abs_ok_6362, seg_count_6363, depth_ok_6364,
 * len_ok_6365, leaf_len_6366, leaf_ok_6367, score_6368, pack_6369,
 * batch_id / wave_ready_6370). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6367_marker[] = "libcgj-batch6367";

#define B6367_LEAF_MIN  1u
#define B6367_LEAF_MAX  255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6367_leaf_ok(uint32_t uLeafLen)
{
	if (uLeafLen < B6367_LEAF_MIN) {
		return 0u;
	}
	if (uLeafLen > B6367_LEAF_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cloud_save_path_leaf_ok_6367 - leaf length in [1, 255] gate.
 *
 * leaf_len: final segment byte length (e.g. from leaf_len_6366)
 *
 * Returns 1 when leaf length is in range, else 0. Soft pure-data only.
 * No parent wires.
 */
uint32_t
gj_cloud_save_path_leaf_ok_6367(uint32_t uLeafLen)
{
	(void)NULL;
	return b6367_leaf_ok(uLeafLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cloud_save_path_leaf_ok_6367(uint32_t uLeafLen)
    __attribute__((alias("gj_cloud_save_path_leaf_ok_6367")));
