/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16299: milestone 16300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_16300(void);
 *     - Returns the exclusive continuum wave id for the milestone 16300
 *       continuum (always 16300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_16300  (alias)
 *   __libcgj_batch16299_marker = "libcgj-batch16299"
 *
 * Milestone 16300 exclusive continuum CREATE-ONLY (16291-16300). Unique
 * gj_continuum_wave_16300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_16200 / gj_continuum_wave_16100 /
 * gj_continuum_wave_16000, gj_milestone_tag_16300 (batch16298), and
 * gj_batch_id_16300 / gj_graph_milestone_16300 (batch16300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (16291–16300): host soft probes
 * (cgj_soft_milestone_16300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=16300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16299. */
const char __libcgj_batch16299_marker[] = "libcgj-batch16299";

/* Exclusive continuum-wave id for milestone 16300. */
#define B16299_WAVE_ID  16300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16299_wave(void)
{
	return B16299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_16300 - report exclusive continuum wave id for 16300.
 *
 * Always returns 16300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 16300.
 */
uint32_t
gj_continuum_wave_16300(void)
{
	(void)NULL;
	return b16299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_16300(void)
    __attribute__((alias("gj_continuum_wave_16300")));
