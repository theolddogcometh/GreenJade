/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87599: milestone 87600 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_87600(void);
 *     - Returns the exclusive continuum wave id for the milestone 87600
 *       continuum (always 87600u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_87600  (alias)
 *   __libcgj_batch87599_marker = "libcgj-batch87599"
 *
 * Milestone 87600 exclusive continuum CREATE-ONLY (25891-87600). Unique
 * gj_continuum_wave_87600 surface only; no multi-def. Distinct from
 * gj_continuum_wave_25800 / gj_continuum_wave_25700 /
 * gj_continuum_wave_25600, gj_milestone_tag_87600 (batch25898), and
 * gj_batch_id_87600 / gj_graph_milestone_87600 (batch87600).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–87600): host soft probes
 * (cgj_soft_milestone_87600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=87600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 87599. */
const char __libcgj_batch87599_marker[] = "libcgj-batch87599";

/* Exclusive continuum-wave id for milestone 87600. */
#define B87599_WAVE_ID  87600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87599_wave(void)
{
	return B87599_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_87600 - report exclusive continuum wave id for 87600.
 *
 * Always returns 87600u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 87600.
 */
uint32_t
gj_continuum_wave_87600(void)
{
	(void)NULL;
	return b87599_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_87600(void)
    __attribute__((alias("gj_continuum_wave_87600")));
