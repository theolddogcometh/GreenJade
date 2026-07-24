/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22398: milestone 22400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_22400(void);
 *     - Returns the milestone tag for the milestone 22400 continuum
 *       (always 22400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_22400  (alias)
 *   __libcgj_batch22398_marker = "libcgj-batch22398"
 *
 * Milestone 22400 exclusive continuum CREATE-ONLY (22391-22400). Unique
 * gj_milestone_tag_22400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_22300 / gj_milestone_tag_22200 / gj_milestone_tag_22100,
 * gj_graph_milestone_22400 (batch22400), and gj_continuum_wave_22400
 * (batch22399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (22391–22400): host soft probes
 * (cgj_soft_milestone_22400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=22400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 22398. */
const char __libcgj_batch22398_marker[] = "libcgj-batch22398";

/* Soft milestone tag for wave 22400 (product identity = 22400). */
#define B22398_MILESTONE_TAG  22400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22398_tag(void)
{
	return B22398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_22400 - report soft milestone product tag for wave 22400.
 *
 * Always returns 22400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 22400.
 */
uint32_t
gj_milestone_tag_22400(void)
{
	(void)NULL;
	return b22398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_22400(void)
    __attribute__((alias("gj_milestone_tag_22400")));
