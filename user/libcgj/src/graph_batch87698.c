/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87698: milestone 87700 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_87700(void);
 *     - Returns the milestone tag for the milestone 87700 continuum
 *       (always 87700u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_87700  (alias)
 *   __libcgj_batch87698_marker = "libcgj-batch87698"
 *
 * Milestone 87700 exclusive continuum CREATE-ONLY (25891-87700). Unique
 * gj_milestone_tag_87700 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_87700 (batch87700), and gj_continuum_wave_87700
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–87700): host soft probes
 * (cgj_soft_milestone_87700.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=87700 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 87698. */
const char __libcgj_batch87698_marker[] = "libcgj-batch87698";

/* Soft milestone tag for wave 87700 (product identity = 87700). */
#define B87698_MILESTONE_TAG  87700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87698_tag(void)
{
	return B87698_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_87700 - report soft milestone product tag for wave 87700.
 *
 * Always returns 87700u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 87700.
 */
uint32_t
gj_milestone_tag_87700(void)
{
	(void)NULL;
	return b87698_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_87700(void)
    __attribute__((alias("gj_milestone_tag_87700")));
