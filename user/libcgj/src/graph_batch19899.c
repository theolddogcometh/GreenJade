/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19899: milestone 19900 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_19900(void);
 *     - Returns the exclusive continuum wave id for the milestone 19900
 *       continuum (always 19900u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_19900  (alias)
 *   __libcgj_batch19899_marker = "libcgj-batch19899"
 *
 * Milestone 19900 exclusive continuum CREATE-ONLY (19891-19900). Unique
 * gj_continuum_wave_19900 surface only; no multi-def. Distinct from
 * gj_continuum_wave_19800 / gj_continuum_wave_19700 /
 * gj_continuum_wave_19600, gj_milestone_tag_19900 (batch19898), and
 * gj_batch_id_19900 / gj_graph_milestone_19900 (batch19900).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (19891–19900): host soft probes
 * (cgj_soft_milestone_19900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=19900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 19899. */
const char __libcgj_batch19899_marker[] = "libcgj-batch19899";

/* Exclusive continuum-wave id for milestone 19900. */
#define B19899_WAVE_ID  19900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19899_wave(void)
{
	return B19899_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_19900 - report exclusive continuum wave id for 19900.
 *
 * Always returns 19900u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 19900.
 */
uint32_t
gj_continuum_wave_19900(void)
{
	(void)NULL;
	return b19899_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_19900(void)
    __attribute__((alias("gj_continuum_wave_19900")));
