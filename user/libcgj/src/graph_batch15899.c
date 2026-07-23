/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15899: milestone 15900 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_15900(void);
 *     - Returns the exclusive continuum wave id for the milestone 15900
 *       continuum (always 15900u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_15900  (alias)
 *   __libcgj_batch15899_marker = "libcgj-batch15899"
 *
 * Milestone 15900 exclusive continuum CREATE-ONLY (15891-15900). Unique
 * gj_continuum_wave_15900 surface only; no multi-def. Distinct from
 * gj_continuum_wave_15800 / gj_continuum_wave_15700 /
 * gj_continuum_wave_15600, gj_milestone_tag_15900 (batch15898), and
 * gj_batch_id_15900 / gj_graph_milestone_15900 (batch15900).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15891–15900): host soft probes
 * (cgj_soft_milestone_15900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15899. */
const char __libcgj_batch15899_marker[] = "libcgj-batch15899";

/* Exclusive continuum-wave id for milestone 15900. */
#define B15899_WAVE_ID  15900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15899_wave(void)
{
	return B15899_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_15900 - report exclusive continuum wave id for 15900.
 *
 * Always returns 15900u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 15900.
 */
uint32_t
gj_continuum_wave_15900(void)
{
	(void)NULL;
	return b15899_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_15900(void)
    __attribute__((alias("gj_continuum_wave_15900")));
