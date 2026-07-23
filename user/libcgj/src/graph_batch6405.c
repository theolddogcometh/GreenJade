/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6405: bar3 install path depth-ok deepen.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_depth_ok_6405(uint32_t seg_count);
 *     - Return 1 if seg_count is in [2, 16] (min/max install-path
 *       depths for this wave), else 0. Soft pure-data depth gate for
 *       the post-6400 bar3 deepen install/steam/top50 continuum.
 *   uint32_t __gj_bar3_install_path_depth_ok_6405  (alias)
 *   __libcgj_batch6405_marker = "libcgj-batch6405"
 *
 * Exclusive continuum CREATE-ONLY (6401-6410: post-6400 bar3 deepen
 * install/steam/top50). Unique gj_bar3_install_path_depth_ok_6405
 * surface only; no multi-def. Distinct from
 * gj_bar3_install_path_depth_ok_6205 (batch6205),
 * gj_bar3_install_path_depth_ok_6005 (batch6005), depth_min_6403 /
 * depth_max_6404 constants, and seg_count_6402. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6405_marker[] = "libcgj-batch6405";

/* Local copies of wave depth bounds (no parent wires). */
#define B6405_DEPTH_MIN  2u
#define B6405_DEPTH_MAX  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6405_depth_ok(uint32_t uSegCount)
{
	if (uSegCount < B6405_DEPTH_MIN) {
		return 0u;
	}
	if (uSegCount > B6405_DEPTH_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_path_depth_ok_6405 - segment depth in [2, 16] gate.
 *
 * seg_count: non-empty path segment count (from seg_count_6402 style)
 *
 * Returns 1 when depth is within the wave bounds, else 0. Soft pure
 * data gate. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_depth_ok_6405(uint32_t uSegCount)
{
	(void)NULL;
	return b6405_depth_ok(uSegCount);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_depth_ok_6405(uint32_t uSegCount)
    __attribute__((alias("gj_bar3_install_path_depth_ok_6405")));
