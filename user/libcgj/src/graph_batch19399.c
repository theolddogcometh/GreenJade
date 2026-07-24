/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19399: milestone 19400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_19400(void);
 *     - Returns the exclusive continuum wave id for the milestone 19400
 *       continuum (always 19400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_19400  (alias)
 *   __libcgj_batch19399_marker = "libcgj-batch19399"
 *
 * Milestone 19400 exclusive continuum CREATE-ONLY (19391-19400). Unique
 * gj_continuum_wave_19400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_19300 / gj_continuum_wave_19200 /
 * gj_continuum_wave_19100, gj_milestone_tag_19400 (batch19398), and
 * gj_batch_id_19400 / gj_graph_milestone_19400 (batch19400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (19391–19400): host soft probes
 * (cgj_soft_milestone_19400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=19400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 19399. */
const char __libcgj_batch19399_marker[] = "libcgj-batch19399";

/* Exclusive continuum-wave id for milestone 19400. */
#define B19399_WAVE_ID  19400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19399_wave(void)
{
	return B19399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_19400 - report exclusive continuum wave id for 19400.
 *
 * Always returns 19400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 19400.
 */
uint32_t
gj_continuum_wave_19400(void)
{
	(void)NULL;
	return b19399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_19400(void)
    __attribute__((alias("gj_continuum_wave_19400")));
