/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6262: fan curve point duty soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pt_duty_ok_6262(uint32_t duty_pct);
 *     - Return 1 if duty_pct is in the soft fan-curve point duty window
 *       [0, 100] (percent), else 0. Pure predicate; not a live PWM
 *       sample.
 *   uint32_t __gj_fc_pt_duty_ok_6262  (alias)
 *   __libcgj_batch6262_marker = "libcgj-batch6262"
 *
 * Exclusive continuum CREATE-ONLY (6261-6270: fan curve point stubs).
 * Unique gj_fc_pt_duty_ok_6262 surface only; no multi-def. Distinct
 * from gj_pwr_fan_curve_u (batch5339) and gj_fc_pt_temp_ok_6261.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6262_marker[] = "libcgj-batch6262";

/* Soft fan-curve control-point duty percent window: [0, 100]. */
#define B6262_DUTY_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6262_duty_ok(uint32_t uDutyPct)
{
	if (uDutyPct > B6262_DUTY_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pt_duty_ok_6262 - 1 if duty_pct is a soft-valid curve point duty.
 *
 * Screens candidate control-point duty percents before packing or PWM
 * conversion. Pure integer predicate. Does not call libc. No parent wires.
 */
uint32_t
gj_fc_pt_duty_ok_6262(uint32_t uDutyPct)
{
	(void)NULL;
	return b6262_duty_ok(uDutyPct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_pt_duty_ok_6262(uint32_t uDutyPct)
    __attribute__((alias("gj_fc_pt_duty_ok_6262")));
