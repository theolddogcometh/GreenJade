/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30899: milestone 30900 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_30900(void);
 *     - Returns the exclusive continuum wave id for the milestone 30900
 *       continuum (always 30900u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_30900  (alias)
 *   __libcgj_batch30899_marker = "libcgj-batch30899"
 *
 * Milestone 30900 exclusive continuum CREATE-ONLY (30891-30900). Unique
 * gj_continuum_wave_30900 surface only; no multi-def. Distinct from
 * gj_continuum_wave_30800 / gj_continuum_wave_30700 /
 * gj_continuum_wave_30600, gj_milestone_tag_30900 (batch30898), and
 * gj_batch_id_30900 / gj_graph_milestone_30900 (batch30900).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30891–30900): host soft probes
 * (cgj_soft_milestone_30900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30899. */
const char __libcgj_batch30899_marker[] = "libcgj-batch30899";

/* Exclusive continuum-wave id for milestone 30900. */
#define B30899_WAVE_ID  30900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30899_wave(void)
{
	return B30899_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_30900 - report exclusive continuum wave id for 30900.
 *
 * Always returns 30900u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 30900.
 */
uint32_t
gj_continuum_wave_30900(void)
{
	(void)NULL;
	return b30899_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_30900(void)
    __attribute__((alias("gj_continuum_wave_30900")));
