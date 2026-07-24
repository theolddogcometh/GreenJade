/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40198: milestone 40200 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_40200(void);
 *     - Returns the milestone tag for the milestone 40200 continuum
 *       (always 40200u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_40200  (alias)
 *   __libcgj_batch40198_marker = "libcgj-batch40198"
 *
 * Milestone 40200 exclusive continuum CREATE-ONLY (40191-40200). Unique
 * gj_milestone_tag_40200 surface only; no multi-def. Distinct from
 * gj_milestone_tag_40100 / gj_milestone_tag_40000 / gj_milestone_tag_39900,
 * gj_graph_milestone_40200 (batch40200), and gj_continuum_wave_40200
 * (batch40199).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40191–40200): host soft probes
 * (cgj_soft_milestone_40200.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=40200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40198. */
const char __libcgj_batch40198_marker[] = "libcgj-batch40198";

/* Soft milestone tag for wave 40200 (product identity = 40200). */
#define B40198_MILESTONE_TAG  40200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40198_tag(void)
{
	return B40198_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_40200 - report soft milestone product tag for wave 40200.
 *
 * Always returns 40200u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 40200.
 */
uint32_t
gj_milestone_tag_40200(void)
{
	(void)NULL;
	return b40198_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_40200(void)
    __attribute__((alias("gj_milestone_tag_40200")));
