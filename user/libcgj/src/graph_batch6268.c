/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6268: fan curve two-point duty lerp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pt_lerp_duty_6268(uint32_t temp_c,
 *                                    uint32_t t0_c, uint32_t d0_pct,
 *                                    uint32_t t1_c, uint32_t d1_pct);
 *     - Linearly interpolate duty between two control points at temp_c:
 *         temp_c <= t0 → d0
 *         temp_c >= t1 → d1
 *         else         → d0 + ((temp_c - t0) * (d1 - d0)) / (t1 - t0)
 *       When t1 <= t0 (degenerate span), return d0. Integer floor div.
 *   uint32_t __gj_fc_pt_lerp_duty_6268  (alias)
 *   __libcgj_batch6268_marker = "libcgj-batch6268"
 *
 * Exclusive continuum CREATE-ONLY (6261-6270: fan curve point stubs).
 * Unique gj_fc_pt_lerp_duty_6268 surface only; no multi-def. Distinct
 * from gj_pwr_fan_curve_u (batch5339 fixed idle/full ramp). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6268_marker[] = "libcgj-batch6268";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Piecewise-linear duty between (t0,d0) and (t1,d1). Assumes callers
 * may pass non-mono points; degenerate t1<=t0 yields d0.
 */
static uint32_t
b6268_lerp(uint32_t uTemp, uint32_t uT0, uint32_t uD0,
    uint32_t uT1, uint32_t uD1)
{
	uint32_t uSpan;
	uint32_t uDeltaT;
	uint32_t uDeltaD;

	if (uT1 <= uT0) {
		return uD0;
	}
	if (uTemp <= uT0) {
		return uD0;
	}
	if (uTemp >= uT1) {
		return uD1;
	}
	uSpan = uT1 - uT0;
	uDeltaT = uTemp - uT0;
	if (uD1 >= uD0) {
		uDeltaD = uD1 - uD0;
		return uD0 + (uDeltaT * uDeltaD) / uSpan;
	}
	/* Falling segment (allowed by this pure lerp; mono is separate). */
	uDeltaD = uD0 - uD1;
	return uD0 - (uDeltaT * uDeltaD) / uSpan;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pt_lerp_duty_6268 - interpolate duty between two curve points.
 *
 * temp_c: query temperature (°C-style)
 * t0_c/d0_pct, t1_c/d1_pct: endpoint control points
 *
 * Integer-only linear interpolation. Does not call libc. No parent wires.
 */
uint32_t
gj_fc_pt_lerp_duty_6268(uint32_t uTempC,
    uint32_t uT0C, uint32_t uD0Pct,
    uint32_t uT1C, uint32_t uD1Pct)
{
	(void)NULL;
	return b6268_lerp(uTempC, uT0C, uD0Pct, uT1C, uD1Pct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_pt_lerp_duty_6268(uint32_t uTempC,
    uint32_t uT0C, uint32_t uD0Pct,
    uint32_t uT1C, uint32_t uD1Pct)
    __attribute__((alias("gj_fc_pt_lerp_duty_6268")));
