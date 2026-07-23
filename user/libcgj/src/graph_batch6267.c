/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6267: fan curve two-point monotonic soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pt_mono_ok_6267(uint32_t t0_c, uint32_t d0_pct,
 *                                  uint32_t t1_c, uint32_t d1_pct);
 *     - Return 1 if the ordered pair of control points is strictly
 *       temperature-increasing (t0_c < t1_c) and non-decreasing in
 *       duty (d0_pct <= d1_pct), else 0. Soft curve-shape check only.
 *   uint32_t __gj_fc_pt_mono_ok_6267  (alias)
 *   __libcgj_batch6267_marker = "libcgj-batch6267"
 *
 * Exclusive continuum CREATE-ONLY (6261-6270: fan curve point stubs).
 * Unique gj_fc_pt_mono_ok_6267 surface only; no multi-def. Distinct
 * from pack / lerp surfaces. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6267_marker[] = "libcgj-batch6267";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6267_mono_ok(uint32_t uT0, uint32_t uD0, uint32_t uT1, uint32_t uD1)
{
	if (uT0 >= uT1) {
		return 0u;
	}
	if (uD0 > uD1) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pt_mono_ok_6267 - 1 if (t0,d0) → (t1,d1) is mono-rising soft-ok.
 *
 * t0_c / t1_c: control-point temperatures (°C-style)
 * d0_pct / d1_pct: control-point duty percents
 *
 * Requires t0 < t1 and d0 <= d1. Pure integer compare. No parent wires.
 */
uint32_t
gj_fc_pt_mono_ok_6267(uint32_t uT0C, uint32_t uD0Pct,
    uint32_t uT1C, uint32_t uD1Pct)
{
	(void)NULL;
	return b6267_mono_ok(uT0C, uD0Pct, uT1C, uD1Pct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_pt_mono_ok_6267(uint32_t uT0C, uint32_t uD0Pct,
    uint32_t uT1C, uint32_t uD1Pct)
    __attribute__((alias("gj_fc_pt_mono_ok_6267")));
