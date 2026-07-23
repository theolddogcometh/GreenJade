/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6408: bar3 install path deepen threshold.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_deepen_threshold_6408(void);
 *     - Returns 60 (soft pass threshold for bar3 install-path deepen
 *       score on the post-6400 bar3 deepen install/steam/top50
 *       continuum). Soft compile-time constant.
 *   uint32_t __gj_bar3_install_path_deepen_threshold_6408  (alias)
 *   __libcgj_batch6408_marker = "libcgj-batch6408"
 *
 * Exclusive continuum CREATE-ONLY (6401-6410: post-6400 bar3 deepen
 * install/steam/top50). Unique
 * gj_bar3_install_path_deepen_threshold_6408 surface only; no multi-def.
 * Distinct from gj_bar3_install_path_deepen_threshold_6208 (batch6208),
 * gj_bar3_install_path_deepen_threshold_6008 (batch6008),
 * gj_top50_path_threshold_6029, and deepen_score_6407. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6408_marker[] = "libcgj-batch6408";

/* Soft pass threshold for install-path deepen score. */
#define B6408_THRESHOLD  60u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6408_threshold(void)
{
	return B6408_THRESHOLD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_path_deepen_threshold_6408 - deepen score pass line.
 *
 * Always returns 60. Soft pure-data constant for deepen_pass_6409.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_deepen_threshold_6408(void)
{
	(void)NULL;
	return b6408_threshold();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_deepen_threshold_6408(void)
    __attribute__((alias("gj_bar3_install_path_deepen_threshold_6408")));
