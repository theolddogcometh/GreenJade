/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49999: milestone 50000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_50000(void);
 *     - Returns the exclusive continuum wave id for the milestone 50000
 *       continuum (always 50000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_50000  (alias)
 *   __libcgj_batch49999_marker = "libcgj-batch49999"
 *
 * Milestone 50000 exclusive continuum CREATE-ONLY (49991-50000). Unique
 * gj_continuum_wave_50000 surface only; no multi-def. Distinct from
 * gj_continuum_wave_49900 / gj_continuum_wave_49800 /
 * gj_continuum_wave_49700, gj_milestone_tag_50000 (batch49998), and
 * gj_batch_id_50000 / gj_graph_milestone_50000 (batch50000).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (49991–50000): host soft probes
 * (cgj_soft_milestone_50000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=50000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 49999. */
const char __libcgj_batch49999_marker[] = "libcgj-batch49999";

/* Exclusive continuum-wave id for milestone 50000. */
#define B49999_WAVE_ID  50000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49999_wave(void)
{
	return B49999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_50000 - report exclusive continuum wave id for 50000.
 *
 * Always returns 50000u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 50000.
 */
uint32_t
gj_continuum_wave_50000(void)
{
	(void)NULL;
	return b49999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_50000(void)
    __attribute__((alias("gj_continuum_wave_50000")));
