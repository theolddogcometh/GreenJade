/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27599: milestone 27600 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_27600(void);
 *     - Returns the exclusive continuum wave id for the milestone 27600
 *       continuum (always 27600u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_27600  (alias)
 *   __libcgj_batch27599_marker = "libcgj-batch27599"
 *
 * Milestone 27600 exclusive continuum CREATE-ONLY (27591-27600). Unique
 * gj_continuum_wave_27600 surface only; no multi-def. Distinct from
 * gj_continuum_wave_27500 / gj_continuum_wave_27400 /
 * gj_continuum_wave_27300, gj_milestone_tag_27600 (batch27598), and
 * gj_batch_id_27600 / gj_graph_milestone_27600 (batch27600).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (27591–27600): host soft probes
 * (cgj_soft_milestone_27600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=27600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 27599. */
const char __libcgj_batch27599_marker[] = "libcgj-batch27599";

/* Exclusive continuum-wave id for milestone 27600. */
#define B27599_WAVE_ID  27600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27599_wave(void)
{
	return B27599_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_27600 - report exclusive continuum wave id for 27600.
 *
 * Always returns 27600u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 27600.
 */
uint32_t
gj_continuum_wave_27600(void)
{
	(void)NULL;
	return b27599_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_27600(void)
    __attribute__((alias("gj_continuum_wave_27600")));
