/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20899: milestone 20900 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_20900(void);
 *     - Returns the exclusive continuum wave id for the milestone 20900
 *       continuum (always 20900u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_20900  (alias)
 *   __libcgj_batch20899_marker = "libcgj-batch20899"
 *
 * Milestone 20900 exclusive continuum CREATE-ONLY (20891-20900). Unique
 * gj_continuum_wave_20900 surface only; no multi-def. Distinct from
 * gj_continuum_wave_20800 / gj_continuum_wave_20700 /
 * gj_continuum_wave_20600, gj_milestone_tag_20900 (batch20898), and
 * gj_batch_id_20900 / gj_graph_milestone_20900 (batch20900).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20891–20900): host soft probes
 * (cgj_soft_milestone_20900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20899. */
const char __libcgj_batch20899_marker[] = "libcgj-batch20899";

/* Exclusive continuum-wave id for milestone 20900. */
#define B20899_WAVE_ID  20900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20899_wave(void)
{
	return B20899_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_20900 - report exclusive continuum wave id for 20900.
 *
 * Always returns 20900u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 20900.
 */
uint32_t
gj_continuum_wave_20900(void)
{
	(void)NULL;
	return b20899_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_20900(void)
    __attribute__((alias("gj_continuum_wave_20900")));
