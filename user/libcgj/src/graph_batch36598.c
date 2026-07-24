/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36598: milestone 36600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_36600(void);
 *     - Returns the milestone tag for the milestone 36600 continuum
 *       (always 36600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_36600  (alias)
 *   __libcgj_batch36598_marker = "libcgj-batch36598"
 *
 * Milestone 36600 exclusive continuum CREATE-ONLY (36591-36600). Unique
 * gj_milestone_tag_36600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_36500 / gj_milestone_tag_36400 / gj_milestone_tag_36300,
 * gj_graph_milestone_36600 (batch36600), and gj_continuum_wave_36600
 * (batch36599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36591–36600): host soft probes
 * (cgj_soft_milestone_36600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36598. */
const char __libcgj_batch36598_marker[] = "libcgj-batch36598";

/* Soft milestone tag for wave 36600 (product identity = 36600). */
#define B36598_MILESTONE_TAG  36600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36598_tag(void)
{
	return B36598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_36600 - report soft milestone product tag for wave 36600.
 *
 * Always returns 36600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 36600.
 */
uint32_t
gj_milestone_tag_36600(void)
{
	(void)NULL;
	return b36598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_36600(void)
    __attribute__((alias("gj_milestone_tag_36600")));
