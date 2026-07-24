/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21999: milestone 22000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_22000(void);
 *     - Returns the exclusive continuum wave id for the milestone 22000
 *       continuum (always 22000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_22000  (alias)
 *   __libcgj_batch21999_marker = "libcgj-batch21999"
 *
 * Milestone 22000 exclusive continuum CREATE-ONLY (21991-22000). Unique
 * gj_continuum_wave_22000 surface only; no multi-def. Distinct from
 * gj_continuum_wave_21900 / gj_continuum_wave_21800 /
 * gj_continuum_wave_21700, gj_milestone_tag_22000 (batch21998), and
 * gj_batch_id_22000 / gj_graph_milestone_22000 (batch22000).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21991–22000): host soft probes
 * (cgj_soft_milestone_22000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=22000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21999. */
const char __libcgj_batch21999_marker[] = "libcgj-batch21999";

/* Exclusive continuum-wave id for milestone 22000. */
#define B21999_WAVE_ID  22000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21999_wave(void)
{
	return B21999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_22000 - report exclusive continuum wave id for 22000.
 *
 * Always returns 22000u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 22000.
 */
uint32_t
gj_continuum_wave_22000(void)
{
	(void)NULL;
	return b21999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_22000(void)
    __attribute__((alias("gj_continuum_wave_22000")));
