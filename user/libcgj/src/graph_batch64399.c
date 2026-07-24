/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch64399: milestone 64400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_64400(void);
 *     - Returns the exclusive continuum wave id for the milestone 64400
 *       continuum (always 64400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_64400  (alias)
 *   __libcgj_batch64399_marker = "libcgj-batch64399"
 *
 * Milestone 64400 exclusive continuum CREATE-ONLY (25891-64400). Unique
 * gj_continuum_wave_64400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_25800 / gj_continuum_wave_25700 /
 * gj_continuum_wave_25600, gj_milestone_tag_64400 (batch25898), and
 * gj_batch_id_64400 / gj_graph_milestone_64400 (batch64400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–64400): host soft probes
 * (cgj_soft_milestone_64400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=64400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 64399. */
const char __libcgj_batch64399_marker[] = "libcgj-batch64399";

/* Exclusive continuum-wave id for milestone 64400. */
#define B64399_WAVE_ID  64400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b64399_wave(void)
{
	return B64399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_64400 - report exclusive continuum wave id for 64400.
 *
 * Always returns 64400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 64400.
 */
uint32_t
gj_continuum_wave_64400(void)
{
	(void)NULL;
	return b64399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_64400(void)
    __attribute__((alias("gj_continuum_wave_64400")));
