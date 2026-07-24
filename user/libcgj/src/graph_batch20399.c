/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20399: milestone 20400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_20400(void);
 *     - Returns the exclusive continuum wave id for the milestone 20400
 *       continuum (always 20400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_20400  (alias)
 *   __libcgj_batch20399_marker = "libcgj-batch20399"
 *
 * Milestone 20400 exclusive continuum CREATE-ONLY (20391-20400). Unique
 * gj_continuum_wave_20400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_20300 / gj_continuum_wave_20200 /
 * gj_continuum_wave_20100, gj_milestone_tag_20400 (batch20398), and
 * gj_batch_id_20400 / gj_graph_milestone_20400 (batch20400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20391–20400): host soft probes
 * (cgj_soft_milestone_20400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20399. */
const char __libcgj_batch20399_marker[] = "libcgj-batch20399";

/* Exclusive continuum-wave id for milestone 20400. */
#define B20399_WAVE_ID  20400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20399_wave(void)
{
	return B20399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_20400 - report exclusive continuum wave id for 20400.
 *
 * Always returns 20400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 20400.
 */
uint32_t
gj_continuum_wave_20400(void)
{
	(void)NULL;
	return b20399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_20400(void)
    __attribute__((alias("gj_continuum_wave_20400")));
