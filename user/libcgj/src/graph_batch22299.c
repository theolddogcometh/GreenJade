/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22299: milestone 22300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_22300(void);
 *     - Returns the exclusive continuum wave id for the milestone 22300
 *       continuum (always 22300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_22300  (alias)
 *   __libcgj_batch22299_marker = "libcgj-batch22299"
 *
 * Milestone 22300 exclusive continuum CREATE-ONLY (22291-22300). Unique
 * gj_continuum_wave_22300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_22200 / gj_continuum_wave_22100 /
 * gj_continuum_wave_22000, gj_milestone_tag_22300 (batch22298), and
 * gj_batch_id_22300 / gj_graph_milestone_22300 (batch22300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (22291–22300): host soft probes
 * (cgj_soft_milestone_22300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=22300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 22299. */
const char __libcgj_batch22299_marker[] = "libcgj-batch22299";

/* Exclusive continuum-wave id for milestone 22300. */
#define B22299_WAVE_ID  22300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22299_wave(void)
{
	return B22299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_22300 - report exclusive continuum wave id for 22300.
 *
 * Always returns 22300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 22300.
 */
uint32_t
gj_continuum_wave_22300(void)
{
	(void)NULL;
	return b22299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_22300(void)
    __attribute__((alias("gj_continuum_wave_22300")));
