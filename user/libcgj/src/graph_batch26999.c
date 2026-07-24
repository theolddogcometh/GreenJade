/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26999: milestone 27000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_27000(void);
 *     - Returns the exclusive continuum wave id for the milestone 27000
 *       continuum (always 27000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_27000  (alias)
 *   __libcgj_batch26999_marker = "libcgj-batch26999"
 *
 * Milestone 27000 exclusive continuum CREATE-ONLY (26991-27000). Unique
 * gj_continuum_wave_27000 surface only; no multi-def. Distinct from
 * gj_continuum_wave_26900 / gj_continuum_wave_26800 /
 * gj_continuum_wave_26700, gj_milestone_tag_27000 (batch26998), and
 * gj_batch_id_27000 / gj_graph_milestone_27000 (batch27000).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26991–27000): host soft probes
 * (cgj_soft_milestone_27000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=27000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26999. */
const char __libcgj_batch26999_marker[] = "libcgj-batch26999";

/* Exclusive continuum-wave id for milestone 27000. */
#define B26999_WAVE_ID  27000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26999_wave(void)
{
	return B26999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_27000 - report exclusive continuum wave id for 27000.
 *
 * Always returns 27000u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 27000.
 */
uint32_t
gj_continuum_wave_27000(void)
{
	(void)NULL;
	return b26999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_27000(void)
    __attribute__((alias("gj_continuum_wave_27000")));
