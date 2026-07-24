/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87898: milestone 87900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_87900(void);
 *     - Returns the milestone tag for the milestone 87900 continuum
 *       (always 87900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_87900  (alias)
 *   __libcgj_batch87898_marker = "libcgj-batch87898"
 *
 * Milestone 87900 exclusive continuum CREATE-ONLY (25891-87900). Unique
 * gj_milestone_tag_87900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_87900 (batch87900), and gj_continuum_wave_87900
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–87900): host soft probes
 * (cgj_soft_milestone_87900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=87900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 87898. */
const char __libcgj_batch87898_marker[] = "libcgj-batch87898";

/* Soft milestone tag for wave 87900 (product identity = 87900). */
#define B87898_MILESTONE_TAG  87900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87898_tag(void)
{
	return B87898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_87900 - report soft milestone product tag for wave 87900.
 *
 * Always returns 87900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 87900.
 */
uint32_t
gj_milestone_tag_87900(void)
{
	(void)NULL;
	return b87898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_87900(void)
    __attribute__((alias("gj_milestone_tag_87900")));
