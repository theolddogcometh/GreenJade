/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18399: milestone 18400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_18400(void);
 *     - Returns the exclusive continuum wave id for the milestone 18400
 *       continuum (always 18400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_18400  (alias)
 *   __libcgj_batch18399_marker = "libcgj-batch18399"
 *
 * Milestone 18400 exclusive continuum CREATE-ONLY (18391-18400). Unique
 * gj_continuum_wave_18400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_18200 / gj_continuum_wave_18100 /
 * gj_continuum_wave_18000, gj_milestone_tag_18400 (batch18398), and
 * gj_batch_id_18400 / gj_graph_milestone_18400 (batch18400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18391–18400): host soft probes
 * (cgj_soft_milestone_18400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=18400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18399. */
const char __libcgj_batch18399_marker[] = "libcgj-batch18399";

/* Exclusive continuum-wave id for milestone 18400. */
#define B18399_WAVE_ID  18400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18399_wave(void)
{
	return B18399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_18400 - report exclusive continuum wave id for 18400.
 *
 * Always returns 18400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 18400.
 */
uint32_t
gj_continuum_wave_18400(void)
{
	(void)NULL;
	return b18399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_18400(void)
    __attribute__((alias("gj_continuum_wave_18400")));
