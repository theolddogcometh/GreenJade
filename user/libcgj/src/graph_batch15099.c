/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15099: milestone 15100 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_15100(void);
 *     - Returns the exclusive continuum wave id for the milestone 15100
 *       continuum (always 15100u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_15100  (alias)
 *   __libcgj_batch15099_marker = "libcgj-batch15099"
 *
 * Milestone 15100 exclusive continuum CREATE-ONLY (15091-15100). Unique
 * gj_continuum_wave_15100 surface only; no multi-def. Distinct from
 * gj_continuum_wave_15000 / gj_continuum_wave_14900 /
 * gj_continuum_wave_14800, gj_milestone_tag_15100 (batch15098), and
 * gj_batch_id_15100 / gj_graph_milestone_15100 (batch15100).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15091–15100): host soft probes
 * (cgj_soft_milestone_15100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15099. */
const char __libcgj_batch15099_marker[] = "libcgj-batch15099";

/* Exclusive continuum-wave id for milestone 15100. */
#define B15099_WAVE_ID  15100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15099_wave(void)
{
	return B15099_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_15100 - report exclusive continuum wave id for 15100.
 *
 * Always returns 15100u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 15100.
 */
uint32_t
gj_continuum_wave_15100(void)
{
	(void)NULL;
	return b15099_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_15100(void)
    __attribute__((alias("gj_continuum_wave_15100")));
