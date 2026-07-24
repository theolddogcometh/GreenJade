/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25299: milestone 25300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_25300(void);
 *     - Returns the exclusive continuum wave id for the milestone 25300
 *       continuum (always 25300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_25300  (alias)
 *   __libcgj_batch25299_marker = "libcgj-batch25299"
 *
 * Milestone 25300 exclusive continuum CREATE-ONLY (25291-25300). Unique
 * gj_continuum_wave_25300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_25200 / gj_continuum_wave_25100 /
 * gj_continuum_wave_25000, gj_milestone_tag_25300 (batch25298), and
 * gj_batch_id_25300 / gj_graph_milestone_25300 (batch25300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25291–25300): host soft probes
 * (cgj_soft_milestone_25300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25299. */
const char __libcgj_batch25299_marker[] = "libcgj-batch25299";

/* Exclusive continuum-wave id for milestone 25300. */
#define B25299_WAVE_ID  25300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25299_wave(void)
{
	return B25299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_25300 - report exclusive continuum wave id for 25300.
 *
 * Always returns 25300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 25300.
 */
uint32_t
gj_continuum_wave_25300(void)
{
	(void)NULL;
	return b25299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_25300(void)
    __attribute__((alias("gj_continuum_wave_25300")));
