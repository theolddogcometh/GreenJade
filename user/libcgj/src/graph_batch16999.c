/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16999: milestone 17000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_17000(void);
 *     - Returns the exclusive continuum wave id for the milestone 17000
 *       continuum (always 17000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_17000  (alias)
 *   __libcgj_batch16999_marker = "libcgj-batch16999"
 *
 * Milestone 17000 exclusive continuum CREATE-ONLY (16991-17000). Unique
 * gj_continuum_wave_17000 surface only; no multi-def. Distinct from
 * gj_continuum_wave_16900 / gj_continuum_wave_16800 /
 * gj_continuum_wave_16700, gj_milestone_tag_17000 (batch16998), and
 * gj_batch_id_17000 / gj_graph_milestone_17000 (batch17000).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (16991–17000): host soft probes
 * (cgj_soft_milestone_17000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=17000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16999. */
const char __libcgj_batch16999_marker[] = "libcgj-batch16999";

/* Exclusive continuum-wave id for milestone 17000. */
#define B16999_WAVE_ID  17000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16999_wave(void)
{
	return B16999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_17000 - report exclusive continuum wave id for 17000.
 *
 * Always returns 17000u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 17000.
 */
uint32_t
gj_continuum_wave_17000(void)
{
	(void)NULL;
	return b16999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_17000(void)
    __attribute__((alias("gj_continuum_wave_17000")));
