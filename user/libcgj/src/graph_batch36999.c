/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36999: milestone 37000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_37000(void);
 *     - Returns the exclusive continuum wave id for the milestone 37000
 *       continuum (always 37000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_37000  (alias)
 *   __libcgj_batch36999_marker = "libcgj-batch36999"
 *
 * Milestone 37000 exclusive continuum CREATE-ONLY (36991-37000). Unique
 * gj_continuum_wave_37000 surface only; no multi-def. Distinct from
 * gj_continuum_wave_36900 / gj_continuum_wave_36800 /
 * gj_continuum_wave_36700, gj_milestone_tag_37000 (batch36998), and
 * gj_batch_id_37000 / gj_graph_milestone_37000 (batch37000).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36991–37000): host soft probes
 * (cgj_soft_milestone_37000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=37000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36999. */
const char __libcgj_batch36999_marker[] = "libcgj-batch36999";

/* Exclusive continuum-wave id for milestone 37000. */
#define B36999_WAVE_ID  37000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36999_wave(void)
{
	return B36999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_37000 - report exclusive continuum wave id for 37000.
 *
 * Always returns 37000u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 37000.
 */
uint32_t
gj_continuum_wave_37000(void)
{
	(void)NULL;
	return b36999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_37000(void)
    __attribute__((alias("gj_continuum_wave_37000")));
