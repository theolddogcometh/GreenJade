/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20599: milestone 20600 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_20600(void);
 *     - Returns the exclusive continuum wave id for the milestone 20600
 *       continuum (always 20600u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_20600  (alias)
 *   __libcgj_batch20599_marker = "libcgj-batch20599"
 *
 * Milestone 20600 exclusive continuum CREATE-ONLY (20591-20600). Unique
 * gj_continuum_wave_20600 surface only; no multi-def. Distinct from
 * gj_continuum_wave_20500 / gj_continuum_wave_20400 /
 * gj_continuum_wave_20300, gj_milestone_tag_20600 (batch20598), and
 * gj_batch_id_20600 / gj_graph_milestone_20600 (batch20600).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20591–20600): host soft probes
 * (cgj_soft_milestone_20600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20599. */
const char __libcgj_batch20599_marker[] = "libcgj-batch20599";

/* Exclusive continuum-wave id for milestone 20600. */
#define B20599_WAVE_ID  20600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20599_wave(void)
{
	return B20599_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_20600 - report exclusive continuum wave id for 20600.
 *
 * Always returns 20600u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 20600.
 */
uint32_t
gj_continuum_wave_20600(void)
{
	(void)NULL;
	return b20599_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_20600(void)
    __attribute__((alias("gj_continuum_wave_20600")));
