/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3444: dyn smoke soft-green gate (wave 3450).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_3450(int code);
 *     - Return 1 if dyn-smoke code is soft-green: code is in [0, 2]
 *       (PASS, soft-warn, soft-skip). Negatives and codes > 2 fail.
 *       Soft pure-data gate for single cgj_dyn_smoke results on the
 *       milestone 3450 continuum. Unsigned 0/1 form.
 *   uint32_t __gj_dyn_green_3450  (alias)
 *   __libcgj_batch3444_marker = "libcgj-batch3444"
 *
 * Milestone 3450 exclusive continuum CREATE-ONLY (3441-3450). Unique
 * gj_dyn_green_3450 surface only; no multi-def. Distinct from
 * gj_dyn_green_3400 (batch3394), gj_dyn_green_3300 (batch3294),
 * gj_dyn_green_u (batch3096), and gj_smoke_green_3450
 * (batch3443 exact 0). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3444_marker[] = "libcgj-batch3444";

/* Inclusive upper bound of dyn-smoke soft-green band. */
#define B3444_DYN_GREEN_MAX  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if dyn code is in soft-green band [0, B3444_DYN_GREEN_MAX]. */
static uint32_t
b3444_green(int nCode)
{
	if (nCode < 0) {
		return 0u;
	}
	if (nCode > B3444_DYN_GREEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_3450 - 1u iff dyn-smoke code is soft-green.
 *
 * code: dyn-smoke result
 *
 * Soft-green band is [0, 2]. Distinct from static smoke green (exact 0)
 * and paired dyn gates. Does not call libc. No parent wires.
 */
uint32_t
gj_dyn_green_3450(int nCode)
{
	(void)NULL;
	return b3444_green(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_3450(int nCode)
    __attribute__((alias("gj_dyn_green_3450")));
