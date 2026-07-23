/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5338: thermal throttle percent vs critical (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pwr_throttle_pct_u(uint32_t temp, uint32_t crit);
 *     - Linear throttle percent: floor((temp * 100) / crit), clamped to
 *       the closed interval [0, 100]. temp >= crit or crit == 0 → 100
 *       (full throttle / undefined-critical sentinel).
 *   uint32_t __gj_pwr_throttle_pct_u  (alias)
 *   __libcgj_batch5338_marker = "libcgj-batch5338"
 *
 * Exclusive continuum CREATE-ONLY (5331-5340: power unique —
 * pwr_state_s0_u, pwr_state_s3_u, pwr_state_s4_u, pwr_state_s5_u,
 * pwr_is_sleep_u, pwr_is_off_u, pwr_is_working_u, pwr_throttle_pct_u,
 * pwr_fan_curve_u, batch_id_5340). Unique gj_pwr_throttle_pct_u surface
 * only; no multi-def. Distinct from gj_throttle_ratio_milli (batch1749)
 * and gj_battery_pct_clamp (batch1931). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5338_marker[] = "libcgj-batch5338";

/* Full throttle / percent ceiling. */
#define B5338_PCT_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((temp * 100) / crit) with clamp to [0, 100].
 * crit == 0 → 100 (undefined critical → full throttle).
 * Intermediate product is uint64_t so temp * 100 cannot wrap uint32_t.
 */
static uint32_t
b5338_throttle_pct(uint32_t u32Temp, uint32_t u32Crit)
{
	uint64_t u64Prod;
	uint32_t u32Pct;

	if (u32Crit == 0u) {
		return B5338_PCT_MAX;
	}
	if (u32Temp >= u32Crit) {
		return B5338_PCT_MAX;
	}
	u64Prod = (uint64_t)u32Temp * (uint64_t)B5338_PCT_MAX;
	u32Pct = (uint32_t)(u64Prod / (uint64_t)u32Crit);
	if (u32Pct > B5338_PCT_MAX) {
		return B5338_PCT_MAX;
	}
	return u32Pct;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pwr_throttle_pct_u - thermal throttle percent vs critical temp.
 *
 * temp: measured temperature (caller unit, e.g. °C)
 * crit: critical temperature threshold (same unit; 0 → full throttle)
 *
 * Returns floor((temp * 100) / crit) clamped to [0, 100]. When temp is
 * at or above crit, returns 100. Integer-only; no floating point.
 * No parent wires.
 */
uint32_t
gj_pwr_throttle_pct_u(uint32_t u32Temp, uint32_t u32Crit)
{
	(void)NULL;
	return b5338_throttle_pct(u32Temp, u32Crit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pwr_throttle_pct_u(uint32_t u32Temp, uint32_t u32Crit)
    __attribute__((alias("gj_pwr_throttle_pct_u")));
