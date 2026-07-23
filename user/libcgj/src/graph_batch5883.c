/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5883: Deck power-profile default TDP step (mW).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_tdp_step_default_mw_5883(void);
 *     - Returns 15000 (Steam Deck default TDP step in milliwatts = 15 W).
 *       Soft compile-time power-profile product stub; not a live RAPL
 *       or hwmon probe.
 *   uint32_t __gj_deck_tdp_step_default_mw_5883  (alias)
 *   __libcgj_batch5883_marker = "libcgj-batch5883"
 *
 * Exclusive continuum CREATE-ONLY (5881-5890: power profile TDP steps
 * for Deck). Unique gj_deck_tdp_step_default_mw_5883 surface only; no
 * multi-def. Distinct from gj_deck_tdp_step_min_mw_5881 / max_mw_5882
 * and gj_deck_tdp_mw_5451. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5883_marker[] = "libcgj-batch5883";

/* Steam Deck default TDP step in milliwatts (15 W). */
#define B5883_TDP_STEP_DEFAULT_MW  15000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5883_tdp_step_default_mw(void)
{
	return B5883_TDP_STEP_DEFAULT_MW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_tdp_step_default_mw_5883 - report Deck default TDP step (mW).
 *
 * Always returns 15000 (15 W). Soft pure-data power-profile stub; does
 * not read RAPL, sysfs, or hwmon. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_tdp_step_default_mw_5883(void)
{
	(void)NULL;
	return b5883_tdp_step_default_mw();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_tdp_step_default_mw_5883(void)
    __attribute__((alias("gj_deck_tdp_step_default_mw_5883")));
