/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20598: milestone 20600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_20600(void);
 *     - Returns the milestone tag for the milestone 20600 continuum
 *       (always 20600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_20600  (alias)
 *   __libcgj_batch20598_marker = "libcgj-batch20598"
 *
 * Milestone 20600 exclusive continuum CREATE-ONLY (20591-20600). Unique
 * gj_milestone_tag_20600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_20500 / gj_milestone_tag_20400 / gj_milestone_tag_20300,
 * gj_graph_milestone_20600 (batch20600), and gj_continuum_wave_20600
 * (batch20599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20591–20600): host soft probes
 * (cgj_soft_milestone_20600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20598. */
const char __libcgj_batch20598_marker[] = "libcgj-batch20598";

/* Soft milestone tag for wave 20600 (product identity = 20600). */
#define B20598_MILESTONE_TAG  20600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20598_tag(void)
{
	return B20598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_20600 - report soft milestone product tag for wave 20600.
 *
 * Always returns 20600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 20600.
 */
uint32_t
gj_milestone_tag_20600(void)
{
	(void)NULL;
	return b20598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_20600(void)
    __attribute__((alias("gj_milestone_tag_20600")));
