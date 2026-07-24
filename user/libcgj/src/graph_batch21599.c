/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21599: milestone 21600 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_21600(void);
 *     - Returns the exclusive continuum wave id for the milestone 21600
 *       continuum (always 21600u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_21600  (alias)
 *   __libcgj_batch21599_marker = "libcgj-batch21599"
 *
 * Milestone 21600 exclusive continuum CREATE-ONLY (21591-21600). Unique
 * gj_continuum_wave_21600 surface only; no multi-def. Distinct from
 * gj_continuum_wave_21500 / gj_continuum_wave_21400 /
 * gj_continuum_wave_21300, gj_milestone_tag_21600 (batch21598), and
 * gj_batch_id_21600 / gj_graph_milestone_21600 (batch21600).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21591–21600): host soft probes
 * (cgj_soft_milestone_21600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21599. */
const char __libcgj_batch21599_marker[] = "libcgj-batch21599";

/* Exclusive continuum-wave id for milestone 21600. */
#define B21599_WAVE_ID  21600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21599_wave(void)
{
	return B21599_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_21600 - report exclusive continuum wave id for 21600.
 *
 * Always returns 21600u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 21600.
 */
uint32_t
gj_continuum_wave_21600(void)
{
	(void)NULL;
	return b21599_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_21600(void)
    __attribute__((alias("gj_continuum_wave_21600")));
