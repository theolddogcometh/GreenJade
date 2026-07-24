/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29598: milestone 29600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_29600(void);
 *     - Returns the milestone tag for the milestone 29600 continuum
 *       (always 29600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_29600  (alias)
 *   __libcgj_batch29598_marker = "libcgj-batch29598"
 *
 * Milestone 29600 exclusive continuum CREATE-ONLY (29591-29600). Unique
 * gj_milestone_tag_29600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_29500 / gj_milestone_tag_29400 / gj_milestone_tag_29300,
 * gj_graph_milestone_29600 (batch29600), and gj_continuum_wave_29600
 * (batch29599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (29591–29600): host soft probes
 * (cgj_soft_milestone_29600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=29600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 29598. */
const char __libcgj_batch29598_marker[] = "libcgj-batch29598";

/* Soft milestone tag for wave 29600 (product identity = 29600). */
#define B29598_MILESTONE_TAG  29600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29598_tag(void)
{
	return B29598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_29600 - report soft milestone product tag for wave 29600.
 *
 * Always returns 29600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 29600.
 */
uint32_t
gj_milestone_tag_29600(void)
{
	(void)NULL;
	return b29598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_29600(void)
    __attribute__((alias("gj_milestone_tag_29600")));
