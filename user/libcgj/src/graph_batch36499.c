/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36499: milestone 36500 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_36500(void);
 *     - Returns the exclusive continuum wave id for the milestone 36500
 *       continuum (always 36500u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_36500  (alias)
 *   __libcgj_batch36499_marker = "libcgj-batch36499"
 *
 * Milestone 36500 exclusive continuum CREATE-ONLY (36491-36500). Unique
 * gj_continuum_wave_36500 surface only; no multi-def. Distinct from
 * gj_continuum_wave_36400 / gj_continuum_wave_36300 /
 * gj_continuum_wave_36200, gj_milestone_tag_36500 (batch36498), and
 * gj_batch_id_36500 / gj_graph_milestone_36500 (batch36500).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36491–36500): host soft probes
 * (cgj_soft_milestone_36500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36499. */
const char __libcgj_batch36499_marker[] = "libcgj-batch36499";

/* Exclusive continuum-wave id for milestone 36500. */
#define B36499_WAVE_ID  36500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36499_wave(void)
{
	return B36499_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_36500 - report exclusive continuum wave id for 36500.
 *
 * Always returns 36500u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 36500.
 */
uint32_t
gj_continuum_wave_36500(void)
{
	(void)NULL;
	return b36499_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_36500(void)
    __attribute__((alias("gj_continuum_wave_36500")));
