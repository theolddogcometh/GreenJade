/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6269: fan curve duty percent → 8-bit PWM.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pt_pwm_from_duty_6269(uint32_t duty_pct);
 *     - Map a duty percent in [0, 100] to an 8-bit PWM count in
 *       [0, 255]: floor((duty_pct * 255) / 100). duty_pct > 100 is
 *       clamped to 100 first. Pure integer conversion.
 *   uint32_t __gj_fc_pt_pwm_from_duty_6269  (alias)
 *   __libcgj_batch6269_marker = "libcgj-batch6269"
 *
 * Exclusive continuum CREATE-ONLY (6261-6270: fan curve point stubs).
 * Unique gj_fc_pt_pwm_from_duty_6269 surface only; no multi-def.
 * Distinct from gj_pwr_fan_curve_u (batch5339) and duty clamp 6265
 * (self-contained clamp; no parent wires). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6269_marker[] = "libcgj-batch6269";

#define B6269_DUTY_MAX  100u
#define B6269_PWM_MAX   255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6269_pwm(uint32_t uDutyPct)
{
	if (uDutyPct > B6269_DUTY_MAX) {
		uDutyPct = B6269_DUTY_MAX;
	}
	return (uDutyPct * B6269_PWM_MAX) / B6269_DUTY_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pt_pwm_from_duty_6269 - duty percent → 0..255 PWM count.
 *
 * Suitable for soft conversion of a curve-point duty into an 8-bit
 * PWM register value. Integer-only. Does not call libc. No parent wires.
 */
uint32_t
gj_fc_pt_pwm_from_duty_6269(uint32_t uDutyPct)
{
	(void)NULL;
	return b6269_pwm(uDutyPct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_pt_pwm_from_duty_6269(uint32_t uDutyPct)
    __attribute__((alias("gj_fc_pt_pwm_from_duty_6269")));
