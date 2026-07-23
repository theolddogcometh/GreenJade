/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6002: bar3 install path segment count deepen.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_seg_count_6002(const char *path);
 *     - Count non-empty path segments separated by '/'. Empty, NULL,
 *       or pure-slash paths return 0. Soft pure-data depth measure for
 *       the post-6000 bar3 install path deepen wave.
 *   uint32_t __gj_bar3_install_path_seg_count_6002  (alias)
 *   __libcgj_batch6002_marker = "libcgj-batch6002"
 *
 * Exclusive continuum CREATE-ONLY (6001-6010: post-6000 bar3 install
 * path deepen). Unique gj_bar3_install_path_seg_count_6002 surface
 * only; no multi-def. Distinct from gj_path_next_seg (batch2165) and
 * gj_bar3_install_path_abs_ok_6001. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6002_marker[] = "libcgj-batch6002";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6002_seg_count(const char *szPath)
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
 * gj_bar3_install_path_seg_count_6002 - count non-empty path segments.
 *
 * path: NUL-terminated install path (NULL -> 0)
 *
 * Returns the number of '/' -separated non-empty segments. Soft pure
 * data depth measure. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_seg_count_6002(const char *szPath)
{
	(void)NULL;
	return b6002_seg_count(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_seg_count_6002(const char *szPath)
    __attribute__((alias("gj_bar3_install_path_seg_count_6002")));
