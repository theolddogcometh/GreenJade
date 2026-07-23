/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3794: dyn smoke soft-green gate (wave 3800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_3800(int code);
 *     - Return 1 if dyn-smoke code is soft-green: code is in [0, 2]
 *       (PASS, soft-warn, soft-skip). Negatives and codes > 2 fail.
 *       Soft pure-data gate for single cgj_dyn_smoke results on the
 *       milestone 3800 continuum. Unsigned 0/1 form.
 *   uint32_t __gj_dyn_green_3800  (alias)
 *   __libcgj_batch3794_marker = "libcgj-batch3794"
 *
 * Milestone 3800 exclusive continuum CREATE-ONLY (3791-3800). Unique
 * gj_dyn_green_3800 surface only; no multi-def. Distinct from
 * gj_dyn_green_3700 (batch3694), gj_dyn_green_3600 (batch3594),
 * gj_dyn_green_3550 (batch3544), and gj_smoke_green_3800
 * (batch3793 exact 0). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3794_marker[] = "libcgj-batch3794";

/* Inclusive upper bound of dyn-smoke soft-green band. */
#define B3794_DYN_GREEN_MAX  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if dyn code is in soft-green band [0, B3794_DYN_GREEN_MAX]. */
static uint32_t
b3794_green(int nCode)
{
	if (nCode < 0) {
		return 0u;
	}
	if (nCode > B3794_DYN_GREEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_3800 - 1u iff dyn-smoke code is soft-green.
 *
 * code: dyn-smoke result
 *
 * Soft-green band is [0, 2]. Distinct from static smoke green (exact 0)
 * and paired dyn gates. Does not call libc. No parent wires.
 */
uint32_t
gj_dyn_green_3800(int nCode)
{
	(void)NULL;
	return b3794_green(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_3800(int nCode)
    __attribute__((alias("gj_dyn_green_3800")));
