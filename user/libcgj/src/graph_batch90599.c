/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch90599: milestone 90600 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_90600(void);
 *     - Returns the exclusive continuum wave id for the milestone 90600
 *       continuum (always 90600u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_90600  (alias)
 *   __libcgj_batch90599_marker = "libcgj-batch90599"
 *
 * Milestone 90600 exclusive continuum CREATE-ONLY (25891-90600). Unique
 * gj_continuum_wave_90600 surface only; no multi-def. Distinct from
 * gj_continuum_wave_25800 / gj_continuum_wave_25700 /
 * gj_continuum_wave_25600, gj_milestone_tag_90600 (batch25898), and
 * gj_batch_id_90600 / gj_graph_milestone_90600 (batch90600).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–90600): host soft probes
 * (cgj_soft_milestone_90600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=90600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 90599. */
const char __libcgj_batch90599_marker[] = "libcgj-batch90599";

/* Exclusive continuum-wave id for milestone 90600. */
#define B90599_WAVE_ID  90600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b90599_wave(void)
{
	return B90599_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_90600 - report exclusive continuum wave id for 90600.
 *
 * Always returns 90600u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 90600.
 */
uint32_t
gj_continuum_wave_90600(void)
{
	(void)NULL;
	return b90599_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_90600(void)
    __attribute__((alias("gj_continuum_wave_90600")));
