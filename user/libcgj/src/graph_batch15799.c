/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15799: milestone 15800 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_15800(void);
 *     - Returns the exclusive continuum wave id for the milestone 15800
 *       continuum (always 15800u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_15800  (alias)
 *   __libcgj_batch15799_marker = "libcgj-batch15799"
 *
 * Milestone 15800 exclusive continuum CREATE-ONLY (15791-15800). Unique
 * gj_continuum_wave_15800 surface only; no multi-def. Distinct from
 * gj_continuum_wave_15700 / gj_continuum_wave_15600 /
 * gj_continuum_wave_15500, gj_milestone_tag_15800 (batch15798), and
 * gj_batch_id_15800 / gj_graph_milestone_15800 (batch15800).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15791–15800): host soft probes
 * (cgj_soft_milestone_15800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15799. */
const char __libcgj_batch15799_marker[] = "libcgj-batch15799";

/* Exclusive continuum-wave id for milestone 15800. */
#define B15799_WAVE_ID  15800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15799_wave(void)
{
	return B15799_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_15800 - report exclusive continuum wave id for 15800.
 *
 * Always returns 15800u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 15800.
 */
uint32_t
gj_continuum_wave_15800(void)
{
	(void)NULL;
	return b15799_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_15800(void)
    __attribute__((alias("gj_continuum_wave_15800")));
