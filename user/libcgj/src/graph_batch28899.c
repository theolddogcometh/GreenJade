/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28899: milestone 28900 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_28900(void);
 *     - Returns the exclusive continuum wave id for the milestone 28900
 *       continuum (always 28900u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_28900  (alias)
 *   __libcgj_batch28899_marker = "libcgj-batch28899"
 *
 * Milestone 28900 exclusive continuum CREATE-ONLY (28891-28900). Unique
 * gj_continuum_wave_28900 surface only; no multi-def. Distinct from
 * gj_continuum_wave_28800 / gj_continuum_wave_28700 /
 * gj_continuum_wave_28600, gj_milestone_tag_28900 (batch28898), and
 * gj_batch_id_28900 / gj_graph_milestone_28900 (batch28900).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28891–28900): host soft probes
 * (cgj_soft_milestone_28900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28899. */
const char __libcgj_batch28899_marker[] = "libcgj-batch28899";

/* Exclusive continuum-wave id for milestone 28900. */
#define B28899_WAVE_ID  28900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28899_wave(void)
{
	return B28899_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_28900 - report exclusive continuum wave id for 28900.
 *
 * Always returns 28900u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 28900.
 */
uint32_t
gj_continuum_wave_28900(void)
{
	(void)NULL;
	return b28899_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_28900(void)
    __attribute__((alias("gj_continuum_wave_28900")));
