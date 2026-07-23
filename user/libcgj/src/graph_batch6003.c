/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6003: bar3 install path min depth constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_depth_min_6003(void);
 *     - Returns 2 (minimum useful install-path segment depth for the
 *       post-6000 bar3 install path deepen continuum, e.g. /opt/steam
 *       style roots with two components). Soft compile-time constant.
 *   uint32_t __gj_bar3_install_path_depth_min_6003  (alias)
 *   __libcgj_batch6003_marker = "libcgj-batch6003"
 *
 * Exclusive continuum CREATE-ONLY (6001-6010: post-6000 bar3 install
 * path deepen). Unique gj_bar3_install_path_depth_min_6003 surface
 * only; no multi-def. Distinct from gj_bar3_install_path_seg_count_6002
 * and gj_esp_size_min_mib_5509. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6003_marker[] = "libcgj-batch6003";

/* Minimum useful bar3 install-path segment depth. */
#define B6003_DEPTH_MIN  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6003_depth_min(void)
{
	return B6003_DEPTH_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_path_depth_min_6003 - min install-path segment depth.
 *
 * Always returns 2. Soft pure-data constant for depth_ok / deepen
 * scoring in this wave. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_depth_min_6003(void)
{
	(void)NULL;
	return b6003_depth_min();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_depth_min_6003(void)
    __attribute__((alias("gj_bar3_install_path_depth_min_6003")));
