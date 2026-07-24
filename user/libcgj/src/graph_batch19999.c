/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19999: milestone 20000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_20000(void);
 *     - Returns the exclusive continuum wave id for the milestone 20000
 *       continuum (always 20000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_20000  (alias)
 *   __libcgj_batch19999_marker = "libcgj-batch19999"
 *
 * Milestone 20000 exclusive continuum CREATE-ONLY (19991-20000). Unique
 * gj_continuum_wave_20000 surface only; no multi-def. Distinct from
 * gj_continuum_wave_19900 / gj_continuum_wave_19800 /
 * gj_continuum_wave_19700, gj_milestone_tag_20000 (batch19998), and
 * gj_batch_id_20000 / gj_graph_milestone_20000 (batch20000).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (19991–20000): host soft probes
 * (cgj_soft_milestone_20000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 19999. */
const char __libcgj_batch19999_marker[] = "libcgj-batch19999";

/* Exclusive continuum-wave id for milestone 20000. */
#define B19999_WAVE_ID  20000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19999_wave(void)
{
	return B19999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_20000 - report exclusive continuum wave id for 20000.
 *
 * Always returns 20000u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 20000.
 */
uint32_t
gj_continuum_wave_20000(void)
{
	(void)NULL;
	return b19999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_20000(void)
    __attribute__((alias("gj_continuum_wave_20000")));
