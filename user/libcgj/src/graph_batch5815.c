/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5815: bar3 bootstrap finalize phase gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_bootstrap_finalize_ok_5815(uint32_t phase_id);
 *     - Return 1 if phase_id is the finalize/ready slot (6 or 7) for
 *       the bar3/Steam bootstrap phase table; else 0. Soft pure-data
 *       gate; does not execute finalize.
 *   uint32_t __gj_bar3_bootstrap_finalize_ok_5815  (alias)
 *   __libcgj_batch5815_marker = "libcgj-batch5815"
 *
 * Exclusive continuum CREATE-ONLY (5811-5820: product bar3 steam
 * bootstrap finalize). Unique gj_bar3_bootstrap_finalize_ok_5815
 * surface only; no multi-def. Distinct from
 * gj_bar3_steam_boot_phase_5812. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5815_marker[] = "libcgj-batch5815";

/* Finalize handoff = 6, ready = 7 (see batch5812 phase table). */
#define B5815_PHASE_FINALIZE  6u
#define B5815_PHASE_READY     7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5815_finalize_ok(uint32_t u32Phase)
{
	if (u32Phase == B5815_PHASE_FINALIZE ||
	    u32Phase == B5815_PHASE_READY) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_bootstrap_finalize_ok_5815 - true on finalize/ready phases.
 *
 * phase_id: bootstrap phase index (pairs with batch5812 table)
 *
 * Returns 1 when phase is finalize handoff (6) or ready (7), else 0.
 * Soft pure-data gate. No parent wires.
 */
uint32_t
gj_bar3_bootstrap_finalize_ok_5815(uint32_t phase_id)
{
	(void)NULL;
	return b5815_finalize_ok(phase_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_bootstrap_finalize_ok_5815(uint32_t phase_id)
    __attribute__((alias("gj_bar3_bootstrap_finalize_ok_5815")));
