/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24098: milestone 24100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_24100(void);
 *     - Returns the milestone tag for the milestone 24100 continuum
 *       (always 24100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_24100  (alias)
 *   __libcgj_batch24098_marker = "libcgj-batch24098"
 *
 * Milestone 24100 exclusive continuum CREATE-ONLY (24091-24100). Unique
 * gj_milestone_tag_24100 surface only; no multi-def. Distinct from
 * gj_milestone_tag_24000 / gj_milestone_tag_23900 / gj_milestone_tag_23800,
 * gj_graph_milestone_24100 (batch24100), and gj_continuum_wave_24100
 * (batch24099).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (24091–24100): host soft probes
 * (cgj_soft_milestone_24100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=24100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24098. */
const char __libcgj_batch24098_marker[] = "libcgj-batch24098";

/* Soft milestone tag for wave 24100 (product identity = 24100). */
#define B24098_MILESTONE_TAG  24100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24098_tag(void)
{
	return B24098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_24100 - report soft milestone product tag for wave 24100.
 *
 * Always returns 24100u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 24100.
 */
uint32_t
gj_milestone_tag_24100(void)
{
	(void)NULL;
	return b24098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_24100(void)
    __attribute__((alias("gj_milestone_tag_24100")));
