/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25999: milestone 26000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_26000(void);
 *     - Returns the exclusive continuum wave id for the milestone 26000
 *       continuum (always 26000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_26000  (alias)
 *   __libcgj_batch25999_marker = "libcgj-batch25999"
 *
 * Milestone 26000 exclusive continuum CREATE-ONLY (25991-26000). Unique
 * gj_continuum_wave_26000 surface only; no multi-def. Distinct from
 * gj_continuum_wave_25900 / gj_continuum_wave_25800 /
 * gj_continuum_wave_25700, gj_milestone_tag_26000 (batch25998), and
 * gj_batch_id_26000 / gj_graph_milestone_26000 (batch26000).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25991–26000): host soft probes
 * (cgj_soft_milestone_26000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25999. */
const char __libcgj_batch25999_marker[] = "libcgj-batch25999";

/* Exclusive continuum-wave id for milestone 26000. */
#define B25999_WAVE_ID  26000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25999_wave(void)
{
	return B25999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_26000 - report exclusive continuum wave id for 26000.
 *
 * Always returns 26000u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 26000.
 */
uint32_t
gj_continuum_wave_26000(void)
{
	(void)NULL;
	return b25999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_26000(void)
    __attribute__((alias("gj_continuum_wave_26000")));
