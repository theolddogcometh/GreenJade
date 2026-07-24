/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch31299: milestone 31300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_31300(void);
 *     - Returns the exclusive continuum wave id for the milestone 31300
 *       continuum (always 31300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_31300  (alias)
 *   __libcgj_batch31299_marker = "libcgj-batch31299"
 *
 * Milestone 31300 exclusive continuum CREATE-ONLY (31291-31300). Unique
 * gj_continuum_wave_31300 surface only; no multi-def. Distinct from
 * gj_continuum_wave_31200 / gj_continuum_wave_31100 /
 * gj_continuum_wave_31000, gj_milestone_tag_31300 (batch31298), and
 * gj_batch_id_31300 / gj_graph_milestone_31300 (batch31300).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (31291–31300): host soft probes
 * (cgj_soft_milestone_31300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=31300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 31299. */
const char __libcgj_batch31299_marker[] = "libcgj-batch31299";

/* Exclusive continuum-wave id for milestone 31300. */
#define B31299_WAVE_ID  31300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b31299_wave(void)
{
	return B31299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_31300 - report exclusive continuum wave id for 31300.
 *
 * Always returns 31300u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 31300.
 */
uint32_t
gj_continuum_wave_31300(void)
{
	(void)NULL;
	return b31299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_31300(void)
    __attribute__((alias("gj_continuum_wave_31300")));
