/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24599: milestone 24600 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_24600(void);
 *     - Returns the exclusive continuum wave id for the milestone 24600
 *       continuum (always 24600u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_24600  (alias)
 *   __libcgj_batch24599_marker = "libcgj-batch24599"
 *
 * Milestone 24600 exclusive continuum CREATE-ONLY (24591-24600). Unique
 * gj_continuum_wave_24600 surface only; no multi-def. Distinct from
 * gj_continuum_wave_24500 / gj_continuum_wave_24400 /
 * gj_continuum_wave_24300, gj_milestone_tag_24600 (batch24598), and
 * gj_batch_id_24600 / gj_graph_milestone_24600 (batch24600).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (24591–24600): host soft probes
 * (cgj_soft_milestone_24600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=24600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24599. */
const char __libcgj_batch24599_marker[] = "libcgj-batch24599";

/* Exclusive continuum-wave id for milestone 24600. */
#define B24599_WAVE_ID  24600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24599_wave(void)
{
	return B24599_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_24600 - report exclusive continuum wave id for 24600.
 *
 * Always returns 24600u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 24600.
 */
uint32_t
gj_continuum_wave_24600(void)
{
	(void)NULL;
	return b24599_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_24600(void)
    __attribute__((alias("gj_continuum_wave_24600")));
