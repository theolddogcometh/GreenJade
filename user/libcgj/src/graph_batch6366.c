/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6366: cloud save path leaf length stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cloud_save_path_leaf_len_6366(const char *path);
 *     - Return the length in bytes of the final non-empty path segment
 *       (leaf object name). NULL, empty, or pure-slash paths return 0.
 *       Soft pure-data leaf measure for cloud save keys.
 *   uint32_t __gj_cloud_save_path_leaf_len_6366  (alias)
 *   __libcgj_batch6366_marker = "libcgj-batch6366"
 *
 * Exclusive continuum CREATE-ONLY (6361-6370: cloud save path stubs —
 * max_len_6361, abs_ok_6362, seg_count_6363, depth_ok_6364,
 * len_ok_6365, leaf_len_6366, leaf_ok_6367, score_6368, pack_6369,
 * batch_id / wave_ready_6370). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6366_marker[] = "libcgj-batch6366";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6366_leaf_len(const char *szPath)
{
	size_t i;
	size_t uEnd;
	size_t uStart;

	if (szPath == NULL) {
		return 0u;
	}
	/* Find end of string. */
	i = 0u;
	while (szPath[i] != '\0') {
		i++;
	}
	/* Skip trailing slashes. */
	while (i > 0u && szPath[i - 1u] == '/') {
		i--;
	}
	if (i == 0u) {
		return 0u;
	}
	uEnd = i;
	/* Walk back to previous slash or start. */
	while (i > 0u && szPath[i - 1u] != '/') {
		i--;
	}
	uStart = i;
	return (uint32_t)(uEnd - uStart);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cloud_save_path_leaf_len_6366 - length of final non-empty segment.
 *
 * path: NUL-terminated cloud save path (NULL -> 0)
 *
 * Returns leaf component byte length (excluding separators). Soft pure
 * data only. Does not call libc. No parent wires.
 */
uint32_t
gj_cloud_save_path_leaf_len_6366(const char *szPath)
{
	(void)NULL;
	return b6366_leaf_len(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cloud_save_path_leaf_len_6366(const char *szPath)
    __attribute__((alias("gj_cloud_save_path_leaf_len_6366")));
