/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30299: milestone 30300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_30300(void);
 *     - Returns the exclusive continuum wave id for the milestone 30300
 *       continuum (always 30300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_30300  (alias)
 *   __libcgj_batch30299_marker = "libcgj-batch30299"
 *
 * Milestone 30300 exclusive continuum CREATE-ONLY (30291-30300). Unique
 * gj_continuum_wave_30300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_30200 / gj_continuum_wave_30100 /
 * gj_continuum_wave_30000, gj_milestone_tag_30300 (batch30298), and
 * gj_batch_id_30300 / gj_graph_milestone_30300 (batch30300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30291–30300): host soft probes
 * (cgj_soft_milestone_30300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30299. */
const char __libcgj_batch30299_marker[] = "libcgj-batch30299";

/* Exclusive continuum-wave id for milestone 30300. */
#define B30299_WAVE_ID  30300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30299_wave(void)
{
	return B30299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_30300 - report exclusive continuum wave id for 30300.
 *
 * Always returns 30300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 30300.
 */
uint32_t
gj_continuum_wave_30300(void)
{
	(void)NULL;
	return b30299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_30300(void)
    __attribute__((alias("gj_continuum_wave_30300")));
