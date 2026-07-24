/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40399: milestone 40400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_40400(void);
 *     - Returns the exclusive continuum wave id for the milestone 40400
 *       continuum (always 40400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_40400  (alias)
 *   __libcgj_batch40399_marker = "libcgj-batch40399"
 *
 * Milestone 40400 exclusive continuum CREATE-ONLY (40391-40400). Unique
 * gj_continuum_wave_40400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_40300 / gj_continuum_wave_40200 /
 * gj_continuum_wave_40100, gj_milestone_tag_40400 (batch40398), and
 * gj_batch_id_40400 / gj_graph_milestone_40400 (batch40400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40391–40400): host soft probes
 * (cgj_soft_milestone_40400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=40400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40399. */
const char __libcgj_batch40399_marker[] = "libcgj-batch40399";

/* Exclusive continuum-wave id for milestone 40400. */
#define B40399_WAVE_ID  40400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40399_wave(void)
{
	return B40399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_40400 - report exclusive continuum wave id for 40400.
 *
 * Always returns 40400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 40400.
 */
uint32_t
gj_continuum_wave_40400(void)
{
	(void)NULL;
	return b40399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_40400(void)
    __attribute__((alias("gj_continuum_wave_40400")));
