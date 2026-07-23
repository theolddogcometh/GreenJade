/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5886: Deck Power Save profile TDP (mW).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_profile_powersave_tdp_mw_5886(void);
 *     - Returns 7000 (Power Save profile TDP in milliwatts = 7 W).
 *       Soft compile-time power-profile product stub; not a live
 *       platform-profile or ryzenadj write.
 *   uint32_t __gj_deck_profile_powersave_tdp_mw_5886  (alias)
 *   __libcgj_batch5886_marker = "libcgj-batch5886"
 *
 * Exclusive continuum CREATE-ONLY (5881-5890: power profile TDP steps
 * for Deck). Unique gj_deck_profile_powersave_tdp_mw_5886 surface only;
 * no multi-def. Distinct from count/range stubs in 5881-5885.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5886_marker[] = "libcgj-batch5886";

/* Power Save profile TDP in milliwatts (7 W). */
#define B5886_POWERSAVE_TDP_MW  7000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5886_powersave_tdp_mw(void)
{
	return B5886_POWERSAVE_TDP_MW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_profile_powersave_tdp_mw_5886 - Power Save profile TDP (mW).
 *
 * Always returns 7000 (7 W). Soft pure-data power-profile stub; does
 * not program SMU limits. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_profile_powersave_tdp_mw_5886(void)
{
	(void)NULL;
	return b5886_powersave_tdp_mw();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_profile_powersave_tdp_mw_5886(void)
    __attribute__((alias("gj_deck_profile_powersave_tdp_mw_5886")));
