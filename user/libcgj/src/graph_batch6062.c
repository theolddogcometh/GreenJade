/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6062: battery capacity clamp (mAh).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batt_capacity_clamp_6062(uint32_t cap_mah);
 *     - Clamp a soft battery capacity (milliamp-hours) into
 *       [0, 20000]. Values above 20000 become 20000.
 *   uint32_t __gj_batt_capacity_clamp_6062  (alias)
 *   __libcgj_batch6062_marker = "libcgj-batch6062"
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

const char __libcgj_batch6062_marker[] = "libcgj-batch6062";

/* Soft max capacity for product stubs (20 Ah in mAh). */
#define B6062_CAP_MAX_MAH  20000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6062_cap_clamp(uint32_t uCapMah)
{
	if (uCapMah > B6062_CAP_MAX_MAH) {
		return B6062_CAP_MAX_MAH;
	}
	return uCapMah;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batt_capacity_clamp_6062 - clamp capacity (mAh) to [0, 20000].
 *
 * cap_mah: design or present capacity in milliamp-hours.
 * Returns cap_mah when <= 20000, else 20000. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_batt_capacity_clamp_6062(uint32_t uCapMah)
{
	(void)NULL;
	return b6062_cap_clamp(uCapMah);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batt_capacity_clamp_6062(uint32_t uCapMah)
    __attribute__((alias("gj_batt_capacity_clamp_6062")));
