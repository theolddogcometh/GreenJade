/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5889: Deck TDP step clamp (mW).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_tdp_step_clamp_mw_5889(uint32_t tdp_mw);
 *     - Clamp tdp_mw into the soft Deck step range [3000, 15000] mW,
 *       then snap down to the nearest 1000 mW (1 W) step. Soft
 *       pure-data helper; does not program firmware.
 *   uint32_t __gj_deck_tdp_step_clamp_mw_5889  (alias)
 *   __libcgj_batch5889_marker = "libcgj-batch5889"
 *
 * Exclusive continuum CREATE-ONLY (5881-5890: power profile TDP steps
 * for Deck). Unique gj_deck_tdp_step_clamp_mw_5889 surface only; no
 * multi-def. Distinct from profile TDP constants in 5886-5888 and
 * range stubs in 5881-5884. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5889_marker[] = "libcgj-batch5889";

/* Soft Deck TDP step range and granularity (mW). Local to this TU. */
#define B5889_TDP_MIN_MW   3000u
#define B5889_TDP_MAX_MW   15000u
#define B5889_TDP_GRAN_MW  1000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5889_clamp_step_mw(uint32_t uMw)
{
	uint32_t uClamped;
	uint32_t uSnapped;

	if (uMw < B5889_TDP_MIN_MW) {
		uClamped = B5889_TDP_MIN_MW;
	} else if (uMw > B5889_TDP_MAX_MW) {
		uClamped = B5889_TDP_MAX_MW;
	} else {
		uClamped = uMw;
	}

	/* Snap down to nearest 1 W step; range is gran-aligned. */
	uSnapped = (uClamped / B5889_TDP_GRAN_MW) * B5889_TDP_GRAN_MW;
	if (uSnapped < B5889_TDP_MIN_MW) {
		return B5889_TDP_MIN_MW;
	}
	return uSnapped;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_tdp_step_clamp_mw_5889 - clamp TDP mW into Deck step grid.
 *
 * tdp_mw: requested thermal design power in milliwatts
 * Returns value in [3000, 15000] snapped down to a 1000 mW step.
 * Soft pure-data helper; does not call libc. No parent wires.
 */
uint32_t
gj_deck_tdp_step_clamp_mw_5889(uint32_t uTdpMw)
{
	(void)NULL;
	return b5889_clamp_step_mw(uTdpMw);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_tdp_step_clamp_mw_5889(uint32_t uTdpMw)
    __attribute__((alias("gj_deck_tdp_step_clamp_mw_5889")));
