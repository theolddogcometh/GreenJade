/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6208: bar3 install path deepen threshold.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_deepen_threshold_6208(void);
 *     - Returns 60 (soft pass threshold for bar3 install-path deepen
 *       score on the post-6200 continuum). Soft compile-time constant.
 *   uint32_t __gj_bar3_install_path_deepen_threshold_6208  (alias)
 *   __libcgj_batch6208_marker = "libcgj-batch6208"
 *
 * Exclusive continuum CREATE-ONLY (6201-6210: post-6200 bar3 install
 * path deepen). Unique gj_bar3_install_path_deepen_threshold_6208
 * surface only; no multi-def. Distinct from
 * gj_bar3_install_path_deepen_threshold_6008 (batch6008),
 * gj_bar3_ready_threshold_5987, and deepen_score_6207. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6208_marker[] = "libcgj-batch6208";

/* Soft pass threshold for install-path deepen score. */
#define B6208_THRESHOLD  60u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6208_threshold(void)
{
	return B6208_THRESHOLD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_path_deepen_threshold_6208 - deepen score pass line.
 *
 * Always returns 60. Soft pure-data constant for deepen_pass_6209.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_deepen_threshold_6208(void)
{
	(void)NULL;
	return b6208_threshold();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_deepen_threshold_6208(void)
    __attribute__((alias("gj_bar3_install_path_deepen_threshold_6208")));
