/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20199: milestone 20200 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_20200(void);
 *     - Returns the exclusive continuum wave id for the milestone 20200
 *       continuum (always 20200u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_20200  (alias)
 *   __libcgj_batch20199_marker = "libcgj-batch20199"
 *
 * Milestone 20200 exclusive continuum CREATE-ONLY (20191-20200). Unique
 * gj_continuum_wave_20200 surface only; no multi-def. Distinct from
 * gj_continuum_wave_20100 / gj_continuum_wave_20000 /
 * gj_continuum_wave_19900, gj_milestone_tag_20200 (batch20198), and
 * gj_batch_id_20200 / gj_graph_milestone_20200 (batch20200).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20191–20200): host soft probes
 * (cgj_soft_milestone_20200.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20199. */
const char __libcgj_batch20199_marker[] = "libcgj-batch20199";

/* Exclusive continuum-wave id for milestone 20200. */
#define B20199_WAVE_ID  20200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20199_wave(void)
{
	return B20199_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_20200 - report exclusive continuum wave id for 20200.
 *
 * Always returns 20200u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 20200.
 */
uint32_t
gj_continuum_wave_20200(void)
{
	(void)NULL;
	return b20199_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_20200(void)
    __attribute__((alias("gj_continuum_wave_20200")));
