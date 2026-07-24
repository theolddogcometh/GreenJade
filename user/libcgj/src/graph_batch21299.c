/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21299: milestone 21300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_21300(void);
 *     - Returns the exclusive continuum wave id for the milestone 21300
 *       continuum (always 21300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_21300  (alias)
 *   __libcgj_batch21299_marker = "libcgj-batch21299"
 *
 * Milestone 21300 exclusive continuum CREATE-ONLY (21291-21300). Unique
 * gj_continuum_wave_21300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_21200 / gj_continuum_wave_21100 /
 * gj_continuum_wave_21000, gj_milestone_tag_21300 (batch21298), and
 * gj_batch_id_21300 / gj_graph_milestone_21300 (batch21300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21291–21300): host soft probes
 * (cgj_soft_milestone_21300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21299. */
const char __libcgj_batch21299_marker[] = "libcgj-batch21299";

/* Exclusive continuum-wave id for milestone 21300. */
#define B21299_WAVE_ID  21300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21299_wave(void)
{
	return B21299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_21300 - report exclusive continuum wave id for 21300.
 *
 * Always returns 21300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 21300.
 */
uint32_t
gj_continuum_wave_21300(void)
{
	(void)NULL;
	return b21299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_21300(void)
    __attribute__((alias("gj_continuum_wave_21300")));
