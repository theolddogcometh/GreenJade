/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18899: milestone 18900 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_18900(void);
 *     - Returns the exclusive continuum wave id for the milestone 18900
 *       continuum (always 18900u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_18900  (alias)
 *   __libcgj_batch18899_marker = "libcgj-batch18899"
 *
 * Milestone 18900 exclusive continuum CREATE-ONLY (18891-18900). Unique
 * gj_continuum_wave_18900 surface only; no multi-def. Distinct from
 * gj_continuum_wave_18700 / gj_continuum_wave_18600 /
 * gj_continuum_wave_18500, gj_milestone_tag_18900 (batch18898), and
 * gj_batch_id_18900 / gj_graph_milestone_18900 (batch18900).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18891–18900): host soft probes
 * (cgj_soft_milestone_18900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=18900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18899. */
const char __libcgj_batch18899_marker[] = "libcgj-batch18899";

/* Exclusive continuum-wave id for milestone 18900. */
#define B18899_WAVE_ID  18900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18899_wave(void)
{
	return B18899_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_18900 - report exclusive continuum wave id for 18900.
 *
 * Always returns 18900u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 18900.
 */
uint32_t
gj_continuum_wave_18900(void)
{
	(void)NULL;
	return b18899_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_18900(void)
    __attribute__((alias("gj_continuum_wave_18900")));
