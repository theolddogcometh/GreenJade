/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5884: Deck power-profile TDP step granularity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_tdp_step_gran_mw_5884(void);
 *     - Returns 1000 (TDP step granularity in milliwatts = 1 W). Soft
 *       compile-time power-profile product stub; not a live firmware
 *       or ryzenadj probe.
 *   uint32_t __gj_deck_tdp_step_gran_mw_5884  (alias)
 *   __libcgj_batch5884_marker = "libcgj-batch5884"
 *
 * Exclusive continuum CREATE-ONLY (5881-5890: power profile TDP steps
 * for Deck). Unique gj_deck_tdp_step_gran_mw_5884 surface only; no
 * multi-def. Distinct from min/max/default step stubs in 5881-5883.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5884_marker[] = "libcgj-batch5884";

/* TDP step granularity in milliwatts (1 W). */
#define B5884_TDP_STEP_GRAN_MW  1000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5884_tdp_step_gran_mw(void)
{
	return B5884_TDP_STEP_GRAN_MW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_tdp_step_gran_mw_5884 - report Deck TDP step granularity (mW).
 *
 * Always returns 1000 (1 W). Soft pure-data power-profile stub; does
 * not program SMU/STAPM. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_tdp_step_gran_mw_5884(void)
{
	(void)NULL;
	return b5884_tdp_step_gran_mw();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_tdp_step_gran_mw_5884(void)
    __attribute__((alias("gj_deck_tdp_step_gran_mw_5884")));
