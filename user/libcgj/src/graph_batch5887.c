/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5887: Deck Balanced profile TDP (mW).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_profile_balanced_tdp_mw_5887(void);
 *     - Returns 10000 (Balanced profile TDP in milliwatts = 10 W).
 *       Soft compile-time power-profile product stub; not a live
 *       platform-profile or ryzenadj write.
 *   uint32_t __gj_deck_profile_balanced_tdp_mw_5887  (alias)
 *   __libcgj_batch5887_marker = "libcgj-batch5887"
 *
 * Exclusive continuum CREATE-ONLY (5881-5890: power profile TDP steps
 * for Deck). Unique gj_deck_profile_balanced_tdp_mw_5887 surface only;
 * no multi-def. Distinct from powersave_tdp_mw_5886 (batch5886).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5887_marker[] = "libcgj-batch5887";

/* Balanced profile TDP in milliwatts (10 W). */
#define B5887_BALANCED_TDP_MW  10000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5887_balanced_tdp_mw(void)
{
	return B5887_BALANCED_TDP_MW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_profile_balanced_tdp_mw_5887 - Balanced profile TDP (mW).
 *
 * Always returns 10000 (10 W). Soft pure-data power-profile stub; does
 * not program SMU limits. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_profile_balanced_tdp_mw_5887(void)
{
	(void)NULL;
	return b5887_balanced_tdp_mw();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_profile_balanced_tdp_mw_5887(void)
    __attribute__((alias("gj_deck_profile_balanced_tdp_mw_5887")));
