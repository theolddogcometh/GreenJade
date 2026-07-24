/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18999: milestone 19000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_19000(void);
 *     - Returns the exclusive continuum wave id for the milestone 19000
 *       continuum (always 19000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_19000  (alias)
 *   __libcgj_batch18999_marker = "libcgj-batch18999"
 *
 * Milestone 19000 exclusive continuum CREATE-ONLY (18991-19000). Unique
 * gj_continuum_wave_19000 surface only; no multi-def. Distinct from
 * gj_continuum_wave_18800 / gj_continuum_wave_18700 /
 * gj_continuum_wave_18600, gj_milestone_tag_19000 (batch18998), and
 * gj_batch_id_19000 / gj_graph_milestone_19000 (batch19000).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18991–19000): host soft probes
 * (cgj_soft_milestone_19000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=19000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18999. */
const char __libcgj_batch18999_marker[] = "libcgj-batch18999";

/* Exclusive continuum-wave id for milestone 19000. */
#define B18999_WAVE_ID  19000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18999_wave(void)
{
	return B18999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_19000 - report exclusive continuum wave id for 19000.
 *
 * Always returns 19000u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 19000.
 */
uint32_t
gj_continuum_wave_19000(void)
{
	(void)NULL;
	return b18999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_19000(void)
    __attribute__((alias("gj_continuum_wave_19000")));
