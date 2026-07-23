/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6515: Steam client binary name length-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scli_bin_len_ok_6515(uint32_t len);
 *     - Return 1 if len is in [1, 64] (min/max Steam client binary
 *       basename lengths for this wave), else 0. Soft pure-data length
 *       gate for the post-6500 steam client deepen continuum.
 *   uint32_t __gj_scli_bin_len_ok_6515  (alias)
 *   __libcgj_batch6515_marker = "libcgj-batch6515"
 *
 * Exclusive continuum CREATE-ONLY (6511-6520: post-6500 steam client
 * deepen). Unique gj_scli_bin_len_ok_6515 surface only; no multi-def.
 * Distinct from bin_min_6513 / bin_max_6514 constants and
 * bin_len_6512. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6515_marker[] = "libcgj-batch6515";

/* Local copies of min/max (no parent wires). */
#define B6515_BIN_MIN  1u
#define B6515_BIN_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6515_len_ok(uint32_t uLen)
{
	if (uLen < B6515_BIN_MIN) {
		return 0u;
	}
	if (uLen > B6515_BIN_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scli_bin_len_ok_6515 - Steam client binary basename length gate.
 *
 * len: measured basename length (typically from bin_len_6512)
 *
 * Returns 1 when len is in [1, 64], else 0. Soft pure-data only.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_scli_bin_len_ok_6515(uint32_t uLen)
{
	(void)NULL;
	return b6515_len_ok(uLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scli_bin_len_ok_6515(uint32_t uLen)
    __attribute__((alias("gj_scli_bin_len_ok_6515")));
