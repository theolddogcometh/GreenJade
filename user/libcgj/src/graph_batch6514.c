/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6514: Steam client binary name max length.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scli_bin_max_6514(void);
 *     - Returns 64 (maximum considered Steam client binary basename
 *       length for the post-6500 steam client deepen continuum). Soft
 *       compile-time constant.
 *   uint32_t __gj_scli_bin_max_6514  (alias)
 *   __libcgj_batch6514_marker = "libcgj-batch6514"
 *
 * Exclusive continuum CREATE-ONLY (6511-6520: post-6500 steam client
 * deepen). Unique gj_scli_bin_max_6514 surface only; no multi-def.
 * Distinct from gj_scli_bin_min_6513 and
 * gj_bar3_install_path_depth_max_6004. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6514_marker[] = "libcgj-batch6514";

/* Maximum considered Steam client binary basename length. */
#define B6514_BIN_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6514_bin_max(void)
{
	return B6514_BIN_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scli_bin_max_6514 - max Steam client binary basename length.
 *
 * Always returns 64. Soft pure-data constant for bin_len_ok / deepen
 * scoring in this wave. Does not call libc. No parent wires.
 */
uint32_t
gj_scli_bin_max_6514(void)
{
	(void)NULL;
	return b6514_bin_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scli_bin_max_6514(void)
    __attribute__((alias("gj_scli_bin_max_6514")));
