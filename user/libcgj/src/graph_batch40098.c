/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40098: milestone 40100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_40100(void);
 *     - Returns the milestone tag for the milestone 40100 continuum
 *       (always 40100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_40100  (alias)
 *   __libcgj_batch40098_marker = "libcgj-batch40098"
 *
 * Milestone 40100 exclusive continuum CREATE-ONLY (40091-40100). Unique
 * gj_milestone_tag_40100 surface only; no multi-def. Distinct from
 * gj_milestone_tag_40000 / gj_milestone_tag_39900 / gj_milestone_tag_39800,
 * gj_graph_milestone_40100 (batch40100), and gj_continuum_wave_40100
 * (batch40099).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40091–40100): host soft probes
 * (cgj_soft_milestone_40100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=40100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40098. */
const char __libcgj_batch40098_marker[] = "libcgj-batch40098";

/* Soft milestone tag for wave 40100 (product identity = 40100). */
#define B40098_MILESTONE_TAG  40100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40098_tag(void)
{
	return B40098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_40100 - report soft milestone product tag for wave 40100.
 *
 * Always returns 40100u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 40100.
 */
uint32_t
gj_milestone_tag_40100(void)
{
	(void)NULL;
	return b40098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_40100(void)
    __attribute__((alias("gj_milestone_tag_40100")));
