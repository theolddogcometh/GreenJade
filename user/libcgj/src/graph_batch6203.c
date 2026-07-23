/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6203: bar3 install path min depth constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_depth_min_6203(void);
 *     - Returns 2 (minimum useful install-path segment depth for the
 *       post-6200 bar3 install path deepen continuum, e.g. /opt/steam
 *       style roots with two components). Soft compile-time constant.
 *   uint32_t __gj_bar3_install_path_depth_min_6203  (alias)
 *   __libcgj_batch6203_marker = "libcgj-batch6203"
 *
 * Exclusive continuum CREATE-ONLY (6201-6210: post-6200 bar3 install
 * path deepen). Unique gj_bar3_install_path_depth_min_6203 surface
 * only; no multi-def. Distinct from gj_bar3_install_path_depth_min_6003
 * (batch6003), gj_bar3_install_path_seg_count_6202, and
 * gj_esp_size_min_mib_5509. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6203_marker[] = "libcgj-batch6203";

/* Minimum useful bar3 install-path segment depth. */
#define B6203_DEPTH_MIN  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6203_depth_min(void)
{
	return B6203_DEPTH_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_path_depth_min_6203 - min install-path segment depth.
 *
 * Always returns 2. Soft pure-data constant for depth_ok / deepen
 * scoring in this wave. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_depth_min_6203(void)
{
	(void)NULL;
	return b6203_depth_min();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_depth_min_6203(void)
    __attribute__((alias("gj_bar3_install_path_depth_min_6203")));
