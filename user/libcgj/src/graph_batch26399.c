/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26399: milestone 26400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_26400(void);
 *     - Returns the exclusive continuum wave id for the milestone 26400
 *       continuum (always 26400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_26400  (alias)
 *   __libcgj_batch26399_marker = "libcgj-batch26399"
 *
 * Milestone 26400 exclusive continuum CREATE-ONLY (26391-26400). Unique
 * gj_continuum_wave_26400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_26300 / gj_continuum_wave_26200 /
 * gj_continuum_wave_26100, gj_milestone_tag_26400 (batch26398), and
 * gj_batch_id_26400 / gj_graph_milestone_26400 (batch26400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26391–26400): host soft probes
 * (cgj_soft_milestone_26400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26399. */
const char __libcgj_batch26399_marker[] = "libcgj-batch26399";

/* Exclusive continuum-wave id for milestone 26400. */
#define B26399_WAVE_ID  26400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26399_wave(void)
{
	return B26399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_26400 - report exclusive continuum wave id for 26400.
 *
 * Always returns 26400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 26400.
 */
uint32_t
gj_continuum_wave_26400(void)
{
	(void)NULL;
	return b26399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_26400(void)
    __attribute__((alias("gj_continuum_wave_26400")));
