/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36399: milestone 36400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_36400(void);
 *     - Returns the exclusive continuum wave id for the milestone 36400
 *       continuum (always 36400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_36400  (alias)
 *   __libcgj_batch36399_marker = "libcgj-batch36399"
 *
 * Milestone 36400 exclusive continuum CREATE-ONLY (36391-36400). Unique
 * gj_continuum_wave_36400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_36300 / gj_continuum_wave_36200 /
 * gj_continuum_wave_36100, gj_milestone_tag_36400 (batch36398), and
 * gj_batch_id_36400 / gj_graph_milestone_36400 (batch36400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36391–36400): host soft probes
 * (cgj_soft_milestone_36400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36399. */
const char __libcgj_batch36399_marker[] = "libcgj-batch36399";

/* Exclusive continuum-wave id for milestone 36400. */
#define B36399_WAVE_ID  36400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36399_wave(void)
{
	return B36399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_36400 - report exclusive continuum wave id for 36400.
 *
 * Always returns 36400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 36400.
 */
uint32_t
gj_continuum_wave_36400(void)
{
	(void)NULL;
	return b36399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_36400(void)
    __attribute__((alias("gj_continuum_wave_36400")));
