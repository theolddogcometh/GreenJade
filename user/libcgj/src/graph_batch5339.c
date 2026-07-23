/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5339: pure-value thermal fan duty curve (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pwr_fan_curve_u(uint32_t temp);
 *     - Map temperature (°C-style) to a fan duty percent in [0, 100]
 *       via a linear ramp: 0 at/below idle (35), 100 at/above full (85),
 *       and floor(((temp - 35) * 100) / 50) in between.
 *   uint32_t __gj_pwr_fan_curve_u  (alias)
 *   __libcgj_batch5339_marker = "libcgj-batch5339"
 *
 * Exclusive continuum CREATE-ONLY (5331-5340: power unique —
 * pwr_state_s0_u, pwr_state_s3_u, pwr_state_s4_u, pwr_state_s5_u,
 * pwr_is_sleep_u, pwr_is_off_u, pwr_is_working_u, pwr_throttle_pct_u,
 * pwr_fan_curve_u, batch_id_5340). Unique gj_pwr_fan_curve_u surface
 * only; no multi-def. Distinct from gj_fan_rpm_ok_hint (batch1936).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5339_marker[] = "libcgj-batch5339";

/* Idle temperature: fan duty 0% at or below this (°C-style). */
#define B5339_TEMP_IDLE  35u
/* Full temperature: fan duty 100% at or above this (°C-style). */
#define B5339_TEMP_FULL  85u
/* Ramp span: FULL - IDLE. */
#define B5339_TEMP_SPAN  50u
/* Fan duty percent ceiling. */
#define B5339_DUTY_MAX   100u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Linear fan duty curve:
 *   temp <= 35 → 0
 *   temp >= 85 → 100
 *   else       → floor(((temp - 35) * 100) / 50)
 */
static uint32_t
b5339_fan_curve(uint32_t u32Temp)
{
	uint32_t u32Delta;
	uint32_t u32Duty;

	if (u32Temp <= B5339_TEMP_IDLE) {
		return 0u;
	}
	if (u32Temp >= B5339_TEMP_FULL) {
		return B5339_DUTY_MAX;
	}
	u32Delta = u32Temp - B5339_TEMP_IDLE;
	u32Duty = (u32Delta * B5339_DUTY_MAX) / B5339_TEMP_SPAN;
	if (u32Duty > B5339_DUTY_MAX) {
		return B5339_DUTY_MAX;
	}
	return u32Duty;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pwr_fan_curve_u - thermal fan duty percent from temperature.
 *
 * temp: measured temperature (°C-style unit)
 *
 * Returns 0 at/below 35, 100 at/above 85, linear in between, always in
 * [0, 100]. Integer-only; no floating point. No parent wires.
 */
uint32_t
gj_pwr_fan_curve_u(uint32_t u32Temp)
{
	(void)NULL;
	return b5339_fan_curve(u32Temp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pwr_fan_curve_u(uint32_t u32Temp)
    __attribute__((alias("gj_pwr_fan_curve_u")));
