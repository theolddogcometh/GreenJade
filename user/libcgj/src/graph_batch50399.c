/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50399: milestone 50400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_50400(void);
 *     - Returns the exclusive continuum wave id for the milestone 50400
 *       continuum (always 50400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_50400  (alias)
 *   __libcgj_batch50399_marker = "libcgj-batch50399"
 *
 * Milestone 50400 exclusive continuum CREATE-ONLY (50391-50400). Unique
 * gj_continuum_wave_50400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_50300 / gj_continuum_wave_50200 /
 * gj_continuum_wave_50100, gj_milestone_tag_50400 (batch50398), and
 * gj_batch_id_50400 / gj_graph_milestone_50400 (batch50400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (50391–50400): host soft probes
 * (cgj_soft_milestone_50400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=50400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 50399. */
const char __libcgj_batch50399_marker[] = "libcgj-batch50399";

/* Exclusive continuum-wave id for milestone 50400. */
#define B50399_WAVE_ID  50400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50399_wave(void)
{
	return B50399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_50400 - report exclusive continuum wave id for 50400.
 *
 * Always returns 50400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 50400.
 */
uint32_t
gj_continuum_wave_50400(void)
{
	(void)NULL;
	return b50399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_50400(void)
    __attribute__((alias("gj_continuum_wave_50400")));
