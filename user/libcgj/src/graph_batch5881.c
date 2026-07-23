/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5881: Deck power-profile TDP step minimum (mW).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_tdp_step_min_mw_5881(void);
 *     - Returns 3000 (Steam Deck soft min TDP step in milliwatts = 3 W).
 *       Soft compile-time power-profile product stub; not a live RAPL
 *       or hwmon probe.
 *   uint32_t __gj_deck_tdp_step_min_mw_5881  (alias)
 *   __libcgj_batch5881_marker = "libcgj-batch5881"
 *
 * Exclusive continuum CREATE-ONLY (5881-5890: power profile TDP steps
 * for Deck — tdp_step_min_mw_5881, tdp_step_max_mw_5882,
 * tdp_step_default_mw_5883, tdp_step_gran_mw_5884,
 * power_profile_count_5885, profile_powersave_tdp_mw_5886,
 * profile_balanced_tdp_mw_5887, profile_perf_tdp_mw_5888,
 * tdp_step_clamp_mw_5889, batch_id_5890). Unique
 * gj_deck_tdp_step_min_mw_5881 surface only; no multi-def. Distinct from
 * gj_deck_tdp_mw_5451 (batch5451). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5881_marker[] = "libcgj-batch5881";

/* Steam Deck soft min TDP step in milliwatts (3 W). */
#define B5881_TDP_STEP_MIN_MW  3000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5881_tdp_step_min_mw(void)
{
	return B5881_TDP_STEP_MIN_MW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_tdp_step_min_mw_5881 - report Deck soft min TDP step (mW).
 *
 * Always returns 3000 (3 W). Soft pure-data power-profile stub; does
 * not read RAPL, sysfs, or hwmon. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_tdp_step_min_mw_5881(void)
{
	(void)NULL;
	return b5881_tdp_step_min_mw();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_tdp_step_min_mw_5881(void)
    __attribute__((alias("gj_deck_tdp_step_min_mw_5881")));
