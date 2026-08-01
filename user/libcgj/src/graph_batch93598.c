/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch93598: milestone 93600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_93600(void);
 *     - Returns the milestone tag for the milestone 93600 continuum
 *       (always 93600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_93600  (alias)
 *   __libcgj_batch93598_marker = "libcgj-batch93598"
 *
 * Milestone 93600 exclusive continuum CREATE-ONLY (25891-93600). Unique
 * gj_milestone_tag_93600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_93600 (batch93600), and gj_continuum_wave_93600
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–93600): host soft probes
 * (cgj_soft_milestone_93600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=93600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 93598. */
const char __libcgj_batch93598_marker[] = "libcgj-batch93598";

/* Soft milestone tag for wave 93600 (product identity = 93600). */
#define B93598_MILESTONE_TAG  93600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b93598_tag(void)
{
	return B93598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_93600 - report soft milestone product tag for wave 93600.
 *
 * Always returns 93600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 93600.
 */
uint32_t
gj_milestone_tag_93600(void)
{
	(void)NULL;
	return b93598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_93600(void)
    __attribute__((alias("gj_milestone_tag_93600")));
