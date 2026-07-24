/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20299: milestone 20300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_20300(void);
 *     - Returns the exclusive continuum wave id for the milestone 20300
 *       continuum (always 20300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_20300  (alias)
 *   __libcgj_batch20299_marker = "libcgj-batch20299"
 *
 * Milestone 20300 exclusive continuum CREATE-ONLY (20291-20300). Unique
 * gj_continuum_wave_20300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_20200 / gj_continuum_wave_20100 /
 * gj_continuum_wave_20000, gj_milestone_tag_20300 (batch20298), and
 * gj_batch_id_20300 / gj_graph_milestone_20300 (batch20300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20291–20300): host soft probes
 * (cgj_soft_milestone_20300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20299. */
const char __libcgj_batch20299_marker[] = "libcgj-batch20299";

/* Exclusive continuum-wave id for milestone 20300. */
#define B20299_WAVE_ID  20300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20299_wave(void)
{
	return B20299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_20300 - report exclusive continuum wave id for 20300.
 *
 * Always returns 20300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 20300.
 */
uint32_t
gj_continuum_wave_20300(void)
{
	(void)NULL;
	return b20299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_20300(void)
    __attribute__((alias("gj_continuum_wave_20300")));
