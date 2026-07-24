/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch31399: milestone 31400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_31400(void);
 *     - Returns the exclusive continuum wave id for the milestone 31400
 *       continuum (always 31400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_31400  (alias)
 *   __libcgj_batch31399_marker = "libcgj-batch31399"
 *
 * Milestone 31400 exclusive continuum CREATE-ONLY (31391-31400). Unique
 * gj_continuum_wave_31400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_31300 / gj_continuum_wave_31200 /
 * gj_continuum_wave_31100, gj_milestone_tag_31400 (batch31398), and
 * gj_batch_id_31400 / gj_graph_milestone_31400 (batch31400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (31391–31400): host soft probes
 * (cgj_soft_milestone_31400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=31400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 31399. */
const char __libcgj_batch31399_marker[] = "libcgj-batch31399";

/* Exclusive continuum-wave id for milestone 31400. */
#define B31399_WAVE_ID  31400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b31399_wave(void)
{
	return B31399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_31400 - report exclusive continuum wave id for 31400.
 *
 * Always returns 31400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 31400.
 */
uint32_t
gj_continuum_wave_31400(void)
{
	(void)NULL;
	return b31399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_31400(void)
    __attribute__((alias("gj_continuum_wave_31400")));
