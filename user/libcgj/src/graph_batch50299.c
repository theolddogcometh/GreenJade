/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50299: milestone 50300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_50300(void);
 *     - Returns the exclusive continuum wave id for the milestone 50300
 *       continuum (always 50300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_50300  (alias)
 *   __libcgj_batch50299_marker = "libcgj-batch50299"
 *
 * Milestone 50300 exclusive continuum CREATE-ONLY (50291-50300). Unique
 * gj_continuum_wave_50300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_50200 / gj_continuum_wave_50100 /
 * gj_continuum_wave_50300, gj_milestone_tag_50300 (batch50298), and
 * gj_batch_id_50300 / gj_graph_milestone_50300 (batch50300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (50291–50300): host soft probes
 * (cgj_soft_milestone_50300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=50300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 50299. */
const char __libcgj_batch50299_marker[] = "libcgj-batch50299";

/* Exclusive continuum-wave id for milestone 50300. */
#define B50299_WAVE_ID  50300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50299_wave(void)
{
	return B50299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_50300 - report exclusive continuum wave id for 50300.
 *
 * Always returns 50300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 50300.
 */
uint32_t
gj_continuum_wave_50300(void)
{
	(void)NULL;
	return b50299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_50300(void)
    __attribute__((alias("gj_continuum_wave_50300")));
