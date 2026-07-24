/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17299: milestone 17300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_17300(void);
 *     - Returns the exclusive continuum wave id for the milestone 17300
 *       continuum (always 17300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_17300  (alias)
 *   __libcgj_batch17299_marker = "libcgj-batch17299"
 *
 * Milestone 17300 exclusive continuum CREATE-ONLY (17291-17300). Unique
 * gj_continuum_wave_17300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_17200 / gj_continuum_wave_17100 /
 * gj_continuum_wave_17000, gj_milestone_tag_17300 (batch17298), and
 * gj_batch_id_17300 / gj_graph_milestone_17300 (batch17300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (17291–17300): host soft probes
 * (cgj_soft_milestone_17300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=17300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 17299. */
const char __libcgj_batch17299_marker[] = "libcgj-batch17299";

/* Exclusive continuum-wave id for milestone 17300. */
#define B17299_WAVE_ID  17300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17299_wave(void)
{
	return B17299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_17300 - report exclusive continuum wave id for 17300.
 *
 * Always returns 17300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 17300.
 */
uint32_t
gj_continuum_wave_17300(void)
{
	(void)NULL;
	return b17299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_17300(void)
    __attribute__((alias("gj_continuum_wave_17300")));
