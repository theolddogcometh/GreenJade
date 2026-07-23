/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6063: battery charge-rate soft-ok (mA).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batt_charge_rate_ok_6063(uint32_t rate_ma);
 *     - Returns 1 if rate_ma is non-zero and within the soft max charge
 *       rate (5000 mA), else 0.
 *   uint32_t __gj_batt_charge_rate_ok_6063  (alias)
 *   __libcgj_batch6063_marker = "libcgj-batch6063"
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

const char __libcgj_batch6063_marker[] = "libcgj-batch6063";

#define B6063_RATE_MAX_MA  5000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6063_rate_ok(uint32_t uRateMa)
{
	return (uRateMa > 0u && uRateMa <= B6063_RATE_MAX_MA) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batt_charge_rate_ok_6063 - 1 if charge rate (mA) is in (0, 5000].
 *
 * rate_ma: charge rate in milliamps.
 * Soft bounds only; does not touch hardware. No parent wires.
 */
uint32_t
gj_batt_charge_rate_ok_6063(uint32_t uRateMa)
{
	(void)NULL;
	return b6063_rate_ok(uRateMa);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batt_charge_rate_ok_6063(uint32_t uRateMa)
    __attribute__((alias("gj_batt_charge_rate_ok_6063")));
