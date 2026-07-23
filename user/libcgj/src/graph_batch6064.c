/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6064: battery capacity soft-ok (mAh).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batt_capacity_ok_6064(uint32_t cap_mah);
 *     - Returns 1 if cap_mah is non-zero and within the soft max
 *       capacity (20000 mAh), else 0.
 *   uint32_t __gj_batt_capacity_ok_6064  (alias)
 *   __libcgj_batch6064_marker = "libcgj-batch6064"
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

const char __libcgj_batch6064_marker[] = "libcgj-batch6064";

#define B6064_CAP_MAX_MAH  20000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6064_cap_ok(uint32_t uCapMah)
{
	return (uCapMah > 0u && uCapMah <= B6064_CAP_MAX_MAH) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batt_capacity_ok_6064 - 1 if capacity (mAh) is in (0, 20000].
 *
 * cap_mah: design or present capacity in milliamp-hours.
 * Soft bounds only; does not touch hardware. No parent wires.
 */
uint32_t
gj_batt_capacity_ok_6064(uint32_t uCapMah)
{
	(void)NULL;
	return b6064_cap_ok(uCapMah);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batt_capacity_ok_6064(uint32_t uCapMah)
    __attribute__((alias("gj_batt_capacity_ok_6064")));
