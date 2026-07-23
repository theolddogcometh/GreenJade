/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6068: charge energy estimate (mWh).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batt_charge_energy_mwh_6068(uint32_t cap_mah,
 *                                           uint32_t volts_mv);
 *     - Floor milliwatt-hours: floor(cap_mah * volts_mv / 1000).
 *       Soft product estimate of stored energy from capacity and pack
 *       voltage. Returns 0 if either input is 0.
 *   uint32_t __gj_batt_charge_energy_mwh_6068  (alias)
 *   __libcgj_batch6068_marker = "libcgj-batch6068"
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

const char __libcgj_batch6068_marker[] = "libcgj-batch6068";

/* Soft max mV for product stubs (60 V). */
#define B6068_MV_MAX  60000u
/* Soft max mAh for product stubs (matches capacity clamp wave). */
#define B6068_MAH_MAX  20000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6068_clamp_mv(uint32_t uMv)
{
	if (uMv > B6068_MV_MAX) {
		return B6068_MV_MAX;
	}
	return uMv;
}

static uint32_t
b6068_clamp_mah(uint32_t uMah)
{
	if (uMah > B6068_MAH_MAX) {
		return B6068_MAH_MAX;
	}
	return uMah;
}

static uint32_t
b6068_energy_mwh(uint32_t uCapMah, uint32_t uVoltsMv)
{
	uint32_t uMah;
	uint32_t uMv;
	uint32_t uProd;

	uMah = b6068_clamp_mah(uCapMah);
	uMv = b6068_clamp_mv(uVoltsMv);
	if (uMah == 0u || uMv == 0u) {
		return 0u;
	}
	/* mAh * mV / 1000 = mWh. Max 20000*60000 = 1.2e9 fits uint32. */
	uProd = uMah * uMv;
	return uProd / 1000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batt_charge_energy_mwh_6068 - floor mWh from capacity and voltage.
 *
 * cap_mah: capacity in milliamp-hours (clamped to 20000).
 * volts_mv: pack voltage in millivolts (clamped to 60000).
 * Returns floor(cap * mv / 1000). Soft pure-integer estimate.
 * No parent wires.
 */
uint32_t
gj_batt_charge_energy_mwh_6068(uint32_t uCapMah, uint32_t uVoltsMv)
{
	(void)NULL;
	return b6068_energy_mwh(uCapMah, uVoltsMv);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batt_charge_energy_mwh_6068(uint32_t uCapMah,
    uint32_t uVoltsMv)
    __attribute__((alias("gj_batt_charge_energy_mwh_6068")));
