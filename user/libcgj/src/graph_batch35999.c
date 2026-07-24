/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35999: milestone 36000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_36000(void);
 *     - Returns the exclusive continuum wave id for the milestone 36000
 *       continuum (always 36000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_36000  (alias)
 *   __libcgj_batch35999_marker = "libcgj-batch35999"
 *
 * Milestone 36000 exclusive continuum CREATE-ONLY (35991-36000). Unique
 * gj_continuum_wave_36000 surface only; no multi-def. Distinct from
 * gj_continuum_wave_35900 / gj_continuum_wave_35800 /
 * gj_continuum_wave_35700, gj_milestone_tag_36000 (batch35998), and
 * gj_batch_id_36000 / gj_graph_milestone_36000 (batch36000).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (35991–36000): host soft probes
 * (cgj_soft_milestone_36000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 35999. */
const char __libcgj_batch35999_marker[] = "libcgj-batch35999";

/* Exclusive continuum-wave id for milestone 36000. */
#define B35999_WAVE_ID  36000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35999_wave(void)
{
	return B35999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_36000 - report exclusive continuum wave id for 36000.
 *
 * Always returns 36000u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 36000.
 */
uint32_t
gj_continuum_wave_36000(void)
{
	(void)NULL;
	return b35999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_36000(void)
    __attribute__((alias("gj_continuum_wave_36000")));
