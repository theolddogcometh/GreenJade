/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6513: Steam client binary name min length.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scli_bin_min_6513(void);
 *     - Returns 1 (minimum useful Steam client binary basename length
 *       for the post-6500 steam client deepen continuum). Soft
 *       compile-time constant.
 *   uint32_t __gj_scli_bin_min_6513  (alias)
 *   __libcgj_batch6513_marker = "libcgj-batch6513"
 *
 * Exclusive continuum CREATE-ONLY (6511-6520: post-6500 steam client
 * deepen). Unique gj_scli_bin_min_6513 surface only; no multi-def.
 * Distinct from gj_scli_bin_len_6512 and
 * gj_bar3_install_path_depth_min_6003. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6513_marker[] = "libcgj-batch6513";

/* Minimum useful Steam client binary basename length. */
#define B6513_BIN_MIN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6513_bin_min(void)
{
	return B6513_BIN_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scli_bin_min_6513 - min Steam client binary basename length.
 *
 * Always returns 1. Soft pure-data constant for bin_len_ok / deepen
 * scoring in this wave. Does not call libc. No parent wires.
 */
uint32_t
gj_scli_bin_min_6513(void)
{
	(void)NULL;
	return b6513_bin_min();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scli_bin_min_6513(void)
    __attribute__((alias("gj_scli_bin_min_6513")));
