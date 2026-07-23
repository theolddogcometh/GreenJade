/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15399: milestone 15400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_15400(void);
 *     - Returns the exclusive continuum wave id for the milestone 15400
 *       continuum (always 15400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_15400  (alias)
 *   __libcgj_batch15399_marker = "libcgj-batch15399"
 *
 * Milestone 15400 exclusive continuum CREATE-ONLY (15391-15400). Unique
 * gj_continuum_wave_15400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_15300 / gj_continuum_wave_15200 /
 * gj_continuum_wave_15100, gj_milestone_tag_15400 (batch15398), and
 * gj_batch_id_15400 / gj_graph_milestone_15400 (batch15400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15391–15400): host soft probes
 * (cgj_soft_milestone_15400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15399. */
const char __libcgj_batch15399_marker[] = "libcgj-batch15399";

/* Exclusive continuum-wave id for milestone 15400. */
#define B15399_WAVE_ID  15400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15399_wave(void)
{
	return B15399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_15400 - report exclusive continuum wave id for 15400.
 *
 * Always returns 15400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 15400.
 */
uint32_t
gj_continuum_wave_15400(void)
{
	(void)NULL;
	return b15399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_15400(void)
    __attribute__((alias("gj_continuum_wave_15400")));
