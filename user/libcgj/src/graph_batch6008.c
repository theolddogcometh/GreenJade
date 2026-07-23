/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6008: bar3 install path deepen threshold.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_path_deepen_threshold_6008(void);
 *     - Returns 60 (soft pass threshold for bar3 install-path deepen
 *       score on the post-6000 continuum). Soft compile-time constant.
 *   uint32_t __gj_bar3_install_path_deepen_threshold_6008  (alias)
 *   __libcgj_batch6008_marker = "libcgj-batch6008"
 *
 * Exclusive continuum CREATE-ONLY (6001-6010: post-6000 bar3 install
 * path deepen). Unique gj_bar3_install_path_deepen_threshold_6008
 * surface only; no multi-def. Distinct from
 * gj_bar3_ready_threshold_5987 and deepen_score_6007. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6008_marker[] = "libcgj-batch6008";

/* Soft pass threshold for install-path deepen score. */
#define B6008_THRESHOLD  60u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6008_threshold(void)
{
	return B6008_THRESHOLD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_path_deepen_threshold_6008 - deepen score pass line.
 *
 * Always returns 60. Soft pure-data constant for deepen_pass_6009.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_install_path_deepen_threshold_6008(void)
{
	(void)NULL;
	return b6008_threshold();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_path_deepen_threshold_6008(void)
    __attribute__((alias("gj_bar3_install_path_deepen_threshold_6008")));
