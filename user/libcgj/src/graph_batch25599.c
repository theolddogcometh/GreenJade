/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25599: milestone 25600 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_25600(void);
 *     - Returns the exclusive continuum wave id for the milestone 25600
 *       continuum (always 25600u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_25600  (alias)
 *   __libcgj_batch25599_marker = "libcgj-batch25599"
 *
 * Milestone 25600 exclusive continuum CREATE-ONLY (25591-25600). Unique
 * gj_continuum_wave_25600 surface only; no multi-def. Distinct from
 * gj_continuum_wave_25500 / gj_continuum_wave_25400 /
 * gj_continuum_wave_25300, gj_milestone_tag_25600 (batch25598), and
 * gj_batch_id_25600 / gj_graph_milestone_25600 (batch25600).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25591–25600): host soft probes
 * (cgj_soft_milestone_25600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25599. */
const char __libcgj_batch25599_marker[] = "libcgj-batch25599";

/* Exclusive continuum-wave id for milestone 25600. */
#define B25599_WAVE_ID  25600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25599_wave(void)
{
	return B25599_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_25600 - report exclusive continuum wave id for 25600.
 *
 * Always returns 25600u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 25600.
 */
uint32_t
gj_continuum_wave_25600(void)
{
	(void)NULL;
	return b25599_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_25600(void)
    __attribute__((alias("gj_continuum_wave_25600")));
