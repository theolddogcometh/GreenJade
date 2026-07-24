/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24598: milestone 24600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_24600(void);
 *     - Returns the milestone tag for the milestone 24600 continuum
 *       (always 24600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_24600  (alias)
 *   __libcgj_batch24598_marker = "libcgj-batch24598"
 *
 * Milestone 24600 exclusive continuum CREATE-ONLY (24591-24600). Unique
 * gj_milestone_tag_24600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_24500 / gj_milestone_tag_24400 / gj_milestone_tag_24300,
 * gj_graph_milestone_24600 (batch24600), and gj_continuum_wave_24600
 * (batch24599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (24591–24600): host soft probes
 * (cgj_soft_milestone_24600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=24600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24598. */
const char __libcgj_batch24598_marker[] = "libcgj-batch24598";

/* Soft milestone tag for wave 24600 (product identity = 24600). */
#define B24598_MILESTONE_TAG  24600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24598_tag(void)
{
	return B24598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_24600 - report soft milestone product tag for wave 24600.
 *
 * Always returns 24600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 24600.
 */
uint32_t
gj_milestone_tag_24600(void)
{
	(void)NULL;
	return b24598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_24600(void)
    __attribute__((alias("gj_milestone_tag_24600")));
