/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35599: milestone 35600 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_35600(void);
 *     - Returns the exclusive continuum wave id for the milestone 35600
 *       continuum (always 35600u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_35600  (alias)
 *   __libcgj_batch35599_marker = "libcgj-batch35599"
 *
 * Milestone 35600 exclusive continuum CREATE-ONLY (35591-35600). Unique
 * gj_continuum_wave_35600 surface only; no multi-def. Distinct from
 * gj_continuum_wave_35500 / gj_continuum_wave_35400 /
 * gj_continuum_wave_35300, gj_milestone_tag_35600 (batch35598), and
 * gj_batch_id_35600 / gj_graph_milestone_35600 (batch35600).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (35591–35600): host soft probes
 * (cgj_soft_milestone_35600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=35600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 35599. */
const char __libcgj_batch35599_marker[] = "libcgj-batch35599";

/* Exclusive continuum-wave id for milestone 35600. */
#define B35599_WAVE_ID  35600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35599_wave(void)
{
	return B35599_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_35600 - report exclusive continuum wave id for 35600.
 *
 * Always returns 35600u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 35600.
 */
uint32_t
gj_continuum_wave_35600(void)
{
	(void)NULL;
	return b35599_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_35600(void)
    __attribute__((alias("gj_continuum_wave_35600")));
