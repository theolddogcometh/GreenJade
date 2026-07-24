/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26299: milestone 26300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_26300(void);
 *     - Returns the exclusive continuum wave id for the milestone 26300
 *       continuum (always 26300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_26300  (alias)
 *   __libcgj_batch26299_marker = "libcgj-batch26299"
 *
 * Milestone 26300 exclusive continuum CREATE-ONLY (26291-26300). Unique
 * gj_continuum_wave_26300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_26200 / gj_continuum_wave_26100 /
 * gj_continuum_wave_26000, gj_milestone_tag_26300 (batch26298), and
 * gj_batch_id_26300 / gj_graph_milestone_26300 (batch26300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26291–26300): host soft probes
 * (cgj_soft_milestone_26300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26299. */
const char __libcgj_batch26299_marker[] = "libcgj-batch26299";

/* Exclusive continuum-wave id for milestone 26300. */
#define B26299_WAVE_ID  26300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26299_wave(void)
{
	return B26299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_26300 - report exclusive continuum wave id for 26300.
 *
 * Always returns 26300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 26300.
 */
uint32_t
gj_continuum_wave_26300(void)
{
	(void)NULL;
	return b26299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_26300(void)
    __attribute__((alias("gj_continuum_wave_26300")));
