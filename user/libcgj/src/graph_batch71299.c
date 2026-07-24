/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch71299: milestone 71300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_71300(void);
 *     - Returns the exclusive continuum wave id for the milestone 71300
 *       continuum (always 71300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_71300  (alias)
 *   __libcgj_batch71299_marker = "libcgj-batch71299"
 *
 * Milestone 71300 exclusive continuum CREATE-ONLY (25891-71300). Unique
 * gj_continuum_wave_71300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_25800 / gj_continuum_wave_25700 /
 * gj_continuum_wave_25600, gj_milestone_tag_71300 (batch25898), and
 * gj_batch_id_71300 / gj_graph_milestone_71300 (batch71300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–71300): host soft probes
 * (cgj_soft_milestone_71300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=71300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 71299. */
const char __libcgj_batch71299_marker[] = "libcgj-batch71299";

/* Exclusive continuum-wave id for milestone 71300. */
#define B71299_WAVE_ID  71300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b71299_wave(void)
{
	return B71299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_71300 - report exclusive continuum wave id for 71300.
 *
 * Always returns 71300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 71300.
 */
uint32_t
gj_continuum_wave_71300(void)
{
	(void)NULL;
	return b71299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_71300(void)
    __attribute__((alias("gj_continuum_wave_71300")));
