/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6261: fan curve point temperature soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pt_temp_ok_6261(uint32_t temp_c);
 *     - Return 1 if temp_c is in the soft fan-curve point temperature
 *       window [0, 120] (°C-style), else 0. Pure predicate; not a live
 *       sensor sample.
 *   uint32_t __gj_fc_pt_temp_ok_6261  (alias)
 *   __libcgj_batch6261_marker = "libcgj-batch6261"
 *
 * Exclusive continuum CREATE-ONLY (6261-6270: fan curve point stubs —
 * temp_ok, duty_ok, pt_ok, clamp_temp, clamp_duty, pack_u, mono_ok,
 * lerp_duty, pwm_from_duty, continuum ready + batch_id_6270). Unique
 * gj_fc_pt_temp_ok_6261 surface only; no multi-def. Distinct from
 * gj_pwr_fan_curve_u (batch5339) and trip temp stubs 6051-6054.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6261_marker[] = "libcgj-batch6261";

/* Soft fan-curve control-point temperature window (°C-style): [0, 120]. */
#define B6261_TEMP_MAX  120u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6261_temp_ok(uint32_t uTempC)
{
	if (uTempC > B6261_TEMP_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pt_temp_ok_6261 - 1 if temp_c is a soft-valid curve point temp.
 *
 * Screens candidate control-point temperatures before packing or lerp.
 * Pure integer predicate. Does not call libc. No parent wires.
 */
uint32_t
gj_fc_pt_temp_ok_6261(uint32_t uTempC)
{
	(void)NULL;
	return b6261_temp_ok(uTempC);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_pt_temp_ok_6261(uint32_t uTempC)
    __attribute__((alias("gj_fc_pt_temp_ok_6261")));
