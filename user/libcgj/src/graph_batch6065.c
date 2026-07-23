/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6065: battery state-of-charge percent.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batt_soc_pct_6065(uint32_t now_mah, uint32_t full_mah);
 *     - Floor percent state of charge: (now_mah * 100) / full_mah when
 *       full_mah != 0 and now_mah <= full_mah; 100 if now > full;
 *       0 if full_mah is 0. Result in [0, 100].
 *   uint32_t __gj_batt_soc_pct_6065  (alias)
 *   __libcgj_batch6065_marker = "libcgj-batch6065"
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

const char __libcgj_batch6065_marker[] = "libcgj-batch6065";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6065_soc_pct(uint32_t uNowMah, uint32_t uFullMah)
{
	uint32_t uProd;

	if (uFullMah == 0u) {
		return 0u;
	}
	if (uNowMah >= uFullMah) {
		return 100u;
	}
	/* now * 100 fits in 32-bit for now <= full <= ~42e6; soft caps
	 * keep product far smaller. */
	uProd = uNowMah * 100u;
	return uProd / uFullMah;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batt_soc_pct_6065 - floor state-of-charge percent from mAh pair.
 *
 * now_mah: present charge in milliamp-hours.
 * full_mah: full/design capacity in milliamp-hours.
 * Returns [0, 100]. Soft pure-integer math. No parent wires.
 */
uint32_t
gj_batt_soc_pct_6065(uint32_t uNowMah, uint32_t uFullMah)
{
	(void)NULL;
	return b6065_soc_pct(uNowMah, uFullMah);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batt_soc_pct_6065(uint32_t uNowMah, uint32_t uFullMah)
    __attribute__((alias("gj_batt_soc_pct_6065")));
