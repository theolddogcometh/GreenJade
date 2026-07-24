/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27999: milestone 28000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_28000(void);
 *     - Returns the exclusive continuum wave id for the milestone 28000
 *       continuum (always 28000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_28000  (alias)
 *   __libcgj_batch27999_marker = "libcgj-batch27999"
 *
 * Milestone 28000 exclusive continuum CREATE-ONLY (27991-28000). Unique
 * gj_continuum_wave_28000 surface only; no multi-def. Distinct from
 * gj_continuum_wave_27900 / gj_continuum_wave_27800 /
 * gj_continuum_wave_27700, gj_milestone_tag_28000 (batch27998), and
 * gj_batch_id_28000 / gj_graph_milestone_28000 (batch28000).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (27991–28000): host soft probes
 * (cgj_soft_milestone_28000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 27999. */
const char __libcgj_batch27999_marker[] = "libcgj-batch27999";

/* Exclusive continuum-wave id for milestone 28000. */
#define B27999_WAVE_ID  28000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27999_wave(void)
{
	return B27999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_28000 - report exclusive continuum wave id for 28000.
 *
 * Always returns 28000u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 28000.
 */
uint32_t
gj_continuum_wave_28000(void)
{
	(void)NULL;
	return b27999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_28000(void)
    __attribute__((alias("gj_continuum_wave_28000")));
