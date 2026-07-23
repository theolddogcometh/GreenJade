/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5451: Steam Deck default TDP stub (mW).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_tdp_mw_5451(void);
 *     - Returns 15000 (Steam Deck default TDP in milliwatts = 15 W).
 *       Soft compile-time power/thermal product stub; not a live RAPL
 *       or hwmon probe.
 *   uint32_t __gj_deck_tdp_mw_5451  (alias)
 *   __libcgj_batch5451_marker = "libcgj-batch5451"
 *
 * Exclusive continuum CREATE-ONLY (5451-5460: Deck power/thermal and
 * display stubs unique — deck_tdp_mw_5451, deck_battery_pct_5452,
 * deck_fan_rpm_5453, deck_refresh_hz_5454, deck_hdr_capable_5455,
 * deck_vrr_capable_5456, deck_brightness_pct_5457, deck_ambient_lux_5458,
 * deck_thermal_ok_5459, batch_id_5460). Unique gj_deck_tdp_mw_5451
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5451_marker[] = "libcgj-batch5451";

/* Steam Deck default TDP in milliwatts (15 W). */
#define B5451_TDP_MW  15000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5451_tdp_mw(void)
{
	return B5451_TDP_MW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_tdp_mw_5451 - report Steam Deck default TDP in milliwatts.
 *
 * Always returns 15000 (15 W). Soft pure-data power/thermal stub; does
 * not read RAPL, sysfs, or hwmon. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_tdp_mw_5451(void)
{
	(void)NULL;
	return b5451_tdp_mw();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_tdp_mw_5451(void)
    __attribute__((alias("gj_deck_tdp_mw_5451")));
