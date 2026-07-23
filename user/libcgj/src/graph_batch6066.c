/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6066: battery remaining capacity (mAh).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batt_remain_mah_6066(uint32_t full_mah, uint32_t soc_pct);
 *     - Remaining mAh from full capacity and SOC percent:
 *       floor(full_mah * clamp(soc_pct,0..100) / 100).
 *   uint32_t __gj_batt_remain_mah_6066  (alias)
 *   __libcgj_batch6066_marker = "libcgj-batch6066"
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

const char __libcgj_batch6066_marker[] = "libcgj-batch6066";

#define B6066_SOC_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6066_clamp_soc(uint32_t uSoc)
{
	if (uSoc > B6066_SOC_MAX) {
		return B6066_SOC_MAX;
	}
	return uSoc;
}

static uint32_t
b6066_remain(uint32_t uFullMah, uint32_t uSocPct)
{
	uint32_t uS;
	uint32_t uProd;

	uS = b6066_clamp_soc(uSocPct);
	if (uFullMah == 0u || uS == 0u) {
		return 0u;
	}
	uProd = uFullMah * uS;
	return uProd / 100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batt_remain_mah_6066 - remaining mAh from full capacity and SOC %.
 *
 * full_mah: full/design capacity in milliamp-hours.
 * soc_pct: state of charge percent (clamped to [0, 100]).
 * Returns floor(full * soc / 100). Soft pure-integer math.
 * No parent wires.
 */
uint32_t
gj_batt_remain_mah_6066(uint32_t uFullMah, uint32_t uSocPct)
{
	(void)NULL;
	return b6066_remain(uFullMah, uSocPct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batt_remain_mah_6066(uint32_t uFullMah, uint32_t uSocPct)
    __attribute__((alias("gj_batt_remain_mah_6066")));
