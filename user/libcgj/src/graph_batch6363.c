/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6363: cloud save path segment count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cloud_save_path_seg_count_6363(const char *path);
 *     - Count non-empty path segments separated by '/'. Empty, NULL,
 *       or pure-slash paths return 0. Soft pure-data depth measure for
 *       the cloud save path continuum.
 *   uint32_t __gj_cloud_save_path_seg_count_6363  (alias)
 *   __libcgj_batch6363_marker = "libcgj-batch6363"
 *
 * Exclusive continuum CREATE-ONLY (6361-6370: cloud save path stubs —
 * max_len_6361, abs_ok_6362, seg_count_6363, depth_ok_6364,
 * len_ok_6365, leaf_len_6366, leaf_ok_6367, score_6368, pack_6369,
 * batch_id / wave_ready_6370). Unique surface only; no multi-def.
 * Distinct from gj_bar3_install_path_seg_count_6202. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6363_marker[] = "libcgj-batch6363";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6363_seg_count(const char *szPath)
{
	uint32_t uCount;
	size_t i;
	int bInSeg;

	if (szPath == NULL) {
		return 0u;
	}
	uCount = 0u;
	bInSeg = 0;
	for (i = 0u; szPath[i] != '\0'; i++) {
		if (szPath[i] == '/') {
			bInSeg = 0;
		} else if (bInSeg == 0) {
			bInSeg = 1;
			if (uCount < UINT32_MAX) {
				uCount++;
			}
		}
	}
	return uCount;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cloud_save_path_seg_count_6363 - count non-empty path segments.
 *
 * path: NUL-terminated cloud save path (NULL -> 0)
 *
 * Returns the number of '/' -separated non-empty segments. Soft pure
 * data depth measure. Does not call libc. No parent wires.
 */
uint32_t
gj_cloud_save_path_seg_count_6363(const char *szPath)
{
	(void)NULL;
	return b6363_seg_count(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cloud_save_path_seg_count_6363(const char *szPath)
    __attribute__((alias("gj_cloud_save_path_seg_count_6363")));
