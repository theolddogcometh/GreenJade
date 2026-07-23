/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6067: minutes to full at charge rate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batt_time_to_full_min_6067(uint32_t remain_mah,
 *                                          uint32_t rate_ma);
 *     - Floor minutes to fill remain_mah at constant rate_ma:
 *       floor(remain_mah * 60 / rate_ma). Returns 0 if rate_ma is 0.
 *       Caps result at 10080 (7 days of minutes) as a soft product bound.
 *   uint32_t __gj_batt_time_to_full_min_6067  (alias)
 *   __libcgj_batch6067_marker = "libcgj-batch6067"
 *
 * Exclusive continuum CREATE-ONLY (6061-6070: battery charge
 * rate/capacity stubs —
 * charge_rate_clamp_6061, capacity_clamp_6062, charge_rate_ok_6063,
 * capacity_ok_6064, soc_pct_6065, remain_mah_6066,
 * time_to_full_min_6067, charge_energy_mwh_6068, charge_ready_6069,
 * continuum + batch_id_6070). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6067_marker[] = "libcgj-batch6067";

/* Soft upper bound: 7 days in minutes. */
#define B6067_MIN_CAP  10080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6067_ttf_min(uint32_t uRemainMah, uint32_t uRateMa)
{
	uint32_t uProd;
	uint32_t uMin;

	if (uRateMa == 0u || uRemainMah == 0u) {
		return 0u;
	}
	/* remain * 60: with soft capacity caps stays well under 2^32. */
	uProd = uRemainMah * 60u;
	uMin = uProd / uRateMa;
	if (uMin > B6067_MIN_CAP) {
		return B6067_MIN_CAP;
	}
	return uMin;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batt_time_to_full_min_6067 - floor minutes to full at rate_ma.
 *
 * remain_mah: charge still needed in milliamp-hours.
 * rate_ma: constant charge rate in milliamps.
 * Returns floor(remain * 60 / rate), capped at 10080; 0 if rate is 0.
 * Soft pure-integer estimate. No parent wires.
 */
uint32_t
gj_batt_time_to_full_min_6067(uint32_t uRemainMah, uint32_t uRateMa)
{
	(void)NULL;
	return b6067_ttf_min(uRemainMah, uRateMa);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batt_time_to_full_min_6067(uint32_t uRemainMah,
    uint32_t uRateMa)
    __attribute__((alias("gj_batt_time_to_full_min_6067")));
