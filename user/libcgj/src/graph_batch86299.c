/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch86299: milestone 86300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_86300(void);
 *     - Returns the exclusive continuum wave id for the milestone 86300
 *       continuum (always 86300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_86300  (alias)
 *   __libcgj_batch86299_marker = "libcgj-batch86299"
 *
 * Milestone 86300 exclusive continuum CREATE-ONLY (25891-86300). Unique
 * gj_continuum_wave_86300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_25800 / gj_continuum_wave_25700 /
 * gj_continuum_wave_25600, gj_milestone_tag_86300 (batch25898), and
 * gj_batch_id_86300 / gj_graph_milestone_86300 (batch86300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–86300): host soft probes
 * (cgj_soft_milestone_86300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=86300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 86299. */
const char __libcgj_batch86299_marker[] = "libcgj-batch86299";

/* Exclusive continuum-wave id for milestone 86300. */
#define B86299_WAVE_ID  86300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b86299_wave(void)
{
	return B86299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_86300 - report exclusive continuum wave id for 86300.
 *
 * Always returns 86300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 86300.
 */
uint32_t
gj_continuum_wave_86300(void)
{
	(void)NULL;
	return b86299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_86300(void)
    __attribute__((alias("gj_continuum_wave_86300")));
