/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25499: milestone 25500 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_25500(void);
 *     - Returns the exclusive continuum wave id for the milestone 25500
 *       continuum (always 25500u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_25500  (alias)
 *   __libcgj_batch25499_marker = "libcgj-batch25499"
 *
 * Milestone 25500 exclusive continuum CREATE-ONLY (25491-25500). Unique
 * gj_continuum_wave_25500 surface only; no multi-def. Distinct from
 * gj_continuum_wave_25400 / gj_continuum_wave_25300 /
 * gj_continuum_wave_25200, gj_milestone_tag_25500 (batch25498), and
 * gj_batch_id_25500 / gj_graph_milestone_25500 (batch25500).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25491–25500): host soft probes
 * (cgj_soft_milestone_25500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25499. */
const char __libcgj_batch25499_marker[] = "libcgj-batch25499";

/* Exclusive continuum-wave id for milestone 25500. */
#define B25499_WAVE_ID  25500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25499_wave(void)
{
	return B25499_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_25500 - report exclusive continuum wave id for 25500.
 *
 * Always returns 25500u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 25500.
 */
uint32_t
gj_continuum_wave_25500(void)
{
	(void)NULL;
	return b25499_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_25500(void)
    __attribute__((alias("gj_continuum_wave_25500")));
