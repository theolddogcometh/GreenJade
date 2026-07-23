/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6404: bar3 install path max depth constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_depth_max_6404(void);
 *     - Returns 16 (maximum considered install-path segment depth for
 *       the post-6400 bar3 deepen install/steam/top50 continuum). Paths
 *       deeper than this are treated as out-of-band for soft scoring.
 *       Soft compile-time constant.
 *   uint32_t __gj_bar3_install_path_depth_max_6404  (alias)
 *   __libcgj_batch6404_marker = "libcgj-batch6404"
 *
 * Exclusive continuum CREATE-ONLY (6401-6410: post-6400 bar3 deepen
 * install/steam/top50). Unique gj_bar3_install_path_depth_max_6404
 * surface only; no multi-def. Distinct from
 * gj_bar3_install_path_depth_max_6204 (batch6204),
 * gj_bar3_install_path_depth_max_6004 (batch6004), and
 * gj_bar3_install_path_depth_min_6403. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6404_marker[] = "libcgj-batch6404";

/* Maximum considered bar3 install-path segment depth. */
#define B6404_DEPTH_MAX  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6404_depth_max(void)
{
	return B6404_DEPTH_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_path_depth_max_6404 - max install-path segment depth.
 *
 * Always returns 16. Soft pure-data constant for depth_ok / deepen
 * scoring in this wave. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_depth_max_6404(void)
{
	(void)NULL;
	return b6404_depth_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_depth_max_6404(void)
    __attribute__((alias("gj_bar3_install_path_depth_max_6404")));
