/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28298: milestone 28300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_28300(void);
 *     - Returns the milestone tag for the milestone 28300 continuum
 *       (always 28300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_28300  (alias)
 *   __libcgj_batch28298_marker = "libcgj-batch28298"
 *
 * Milestone 28300 exclusive continuum CREATE-ONLY (28291-28300). Unique
 * gj_milestone_tag_28300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_28200 / gj_milestone_tag_28100 / gj_milestone_tag_28000,
 * gj_graph_milestone_28300 (batch28300), and gj_continuum_wave_28300
 * (batch28299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28291–28300): host soft probes
 * (cgj_soft_milestone_28300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28298. */
const char __libcgj_batch28298_marker[] = "libcgj-batch28298";

/* Soft milestone tag for wave 28300 (product identity = 28300). */
#define B28298_MILESTONE_TAG  28300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28298_tag(void)
{
	return B28298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_28300 - report soft milestone product tag for wave 28300.
 *
 * Always returns 28300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 28300.
 */
uint32_t
gj_milestone_tag_28300(void)
{
	(void)NULL;
	return b28298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_28300(void)
    __attribute__((alias("gj_milestone_tag_28300")));
