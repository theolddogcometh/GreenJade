/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21399: milestone 21400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_21400(void);
 *     - Returns the exclusive continuum wave id for the milestone 21400
 *       continuum (always 21400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_21400  (alias)
 *   __libcgj_batch21399_marker = "libcgj-batch21399"
 *
 * Milestone 21400 exclusive continuum CREATE-ONLY (21391-21400). Unique
 * gj_continuum_wave_21400 surface only; no multi-def. Distinct from
 * gj_continuum_wave_21300 / gj_continuum_wave_21200 /
 * gj_continuum_wave_21100, gj_milestone_tag_21400 (batch21398), and
 * gj_batch_id_21400 / gj_graph_milestone_21400 (batch21400).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21391–21400): host soft probes
 * (cgj_soft_milestone_21400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21399. */
const char __libcgj_batch21399_marker[] = "libcgj-batch21399";

/* Exclusive continuum-wave id for milestone 21400. */
#define B21399_WAVE_ID  21400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21399_wave(void)
{
	return B21399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_21400 - report exclusive continuum wave id for 21400.
 *
 * Always returns 21400u. Soft pure-data wave identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 21400.
 */
uint32_t
gj_continuum_wave_21400(void)
{
	(void)NULL;
	return b21399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_wave_21400(void)
    __attribute__((alias("gj_continuum_wave_21400")));
