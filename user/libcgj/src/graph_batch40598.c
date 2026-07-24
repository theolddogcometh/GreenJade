/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40598: milestone 40600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_40600(void);
 *     - Returns the milestone tag for the milestone 40600 continuum
 *       (always 40600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_40600  (alias)
 *   __libcgj_batch40598_marker = "libcgj-batch40598"
 *
 * Milestone 40600 exclusive continuum CREATE-ONLY (40591-40600). Unique
 * gj_milestone_tag_40600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_40500 / gj_milestone_tag_40400 / gj_milestone_tag_40300,
 * gj_graph_milestone_40600 (batch40600), and gj_continuum_wave_40600
 * (batch40599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40591–40600): host soft probes
 * (cgj_soft_milestone_40600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=40600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40598. */
const char __libcgj_batch40598_marker[] = "libcgj-batch40598";

/* Soft milestone tag for wave 40600 (product identity = 40600). */
#define B40598_MILESTONE_TAG  40600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40598_tag(void)
{
	return B40598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_40600 - report soft milestone product tag for wave 40600.
 *
 * Always returns 40600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 40600.
 */
uint32_t
gj_milestone_tag_40600(void)
{
	(void)NULL;
	return b40598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_40600(void)
    __attribute__((alias("gj_milestone_tag_40600")));
