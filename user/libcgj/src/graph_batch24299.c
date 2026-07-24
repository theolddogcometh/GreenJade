/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24299: milestone 24300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_24300(void);
 *     - Returns the exclusive continuum wave id for the milestone 24300
 *       continuum (always 24300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_24300  (alias)
 *   __libcgj_batch24299_marker = "libcgj-batch24299"
 *
 * Milestone 24300 exclusive continuum CREATE-ONLY (24291-24300). Unique
 * gj_continuum_wave_24300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_24200 / gj_continuum_wave_24100 /
 * gj_continuum_wave_24000, gj_milestone_tag_24300 (batch24298), and
 * gj_batch_id_24300 / gj_graph_milestone_24300 (batch24300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (24291–24300): host soft probes
 * (cgj_soft_milestone_24300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=24300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24299. */
const char __libcgj_batch24299_marker[] = "libcgj-batch24299";

/* Exclusive continuum-wave id for milestone 24300. */
#define B24299_WAVE_ID  24300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24299_wave(void)
{
	return B24299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_24300 - report exclusive continuum wave id for 24300.
 *
 * Always returns 24300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 24300.
 */
uint32_t
gj_continuum_wave_24300(void)
{
	(void)NULL;
	return b24299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_24300(void)
    __attribute__((alias("gj_continuum_wave_24300")));
