/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5888: Deck Performance profile TDP (mW).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_profile_perf_tdp_mw_5888(void);
 *     - Returns 15000 (Performance profile TDP in milliwatts = 15 W).
 *       Soft compile-time power-profile product stub; not a live
 *       platform-profile or ryzenadj write.
 *   uint32_t __gj_deck_profile_perf_tdp_mw_5888  (alias)
 *   __libcgj_batch5888_marker = "libcgj-batch5888"
 *
 * Exclusive continuum CREATE-ONLY (5881-5890: power profile TDP steps
 * for Deck). Unique gj_deck_profile_perf_tdp_mw_5888 surface only; no
 * multi-def. Distinct from powersave_5886 / balanced_5887.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5888_marker[] = "libcgj-batch5888";

/* Performance profile TDP in milliwatts (15 W). */
#define B5888_PERF_TDP_MW  15000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5888_perf_tdp_mw(void)
{
	return B5888_PERF_TDP_MW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_profile_perf_tdp_mw_5888 - Performance profile TDP (mW).
 *
 * Always returns 15000 (15 W). Soft pure-data power-profile stub; does
 * not program SMU limits. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_profile_perf_tdp_mw_5888(void)
{
	(void)NULL;
	return b5888_perf_tdp_mw();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_profile_perf_tdp_mw_5888(void)
    __attribute__((alias("gj_deck_profile_perf_tdp_mw_5888")));
