/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6061: battery charge-rate clamp (mA).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batt_charge_rate_clamp_6061(uint32_t rate_ma);
 *     - Clamp a soft battery charge rate (milliamps) into [0, 5000].
 *       Values above 5000 become 5000; unsigned so the floor is 0.
 *   uint32_t __gj_batt_charge_rate_clamp_6061  (alias)
 *   __libcgj_batch6061_marker = "libcgj-batch6061"
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

const char __libcgj_batch6061_marker[] = "libcgj-batch6061";

/* Soft max charge rate for product stubs (5 A in mA). */
#define B6061_RATE_MAX_MA  5000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6061_rate_clamp(uint32_t uRateMa)
{
	if (uRateMa > B6061_RATE_MAX_MA) {
		return B6061_RATE_MAX_MA;
	}
	return uRateMa;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batt_charge_rate_clamp_6061 - clamp charge rate (mA) to [0, 5000].
 *
 * rate_ma: raw charge-rate reading or setpoint in milliamps.
 * Returns rate_ma when <= 5000, else 5000. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_batt_charge_rate_clamp_6061(uint32_t uRateMa)
{
	(void)NULL;
	return b6061_rate_clamp(uRateMa);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batt_charge_rate_clamp_6061(uint32_t uRateMa)
    __attribute__((alias("gj_batt_charge_rate_clamp_6061")));
