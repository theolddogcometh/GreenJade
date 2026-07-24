/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24898: milestone 24900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_24900(void);
 *     - Returns the milestone tag for the milestone 24900 continuum
 *       (always 24900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_24900  (alias)
 *   __libcgj_batch24898_marker = "libcgj-batch24898"
 *
 * Milestone 24900 exclusive continuum CREATE-ONLY (24891-24900). Unique
 * gj_milestone_tag_24900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_24800 / gj_milestone_tag_24700 / gj_milestone_tag_24500,
 * gj_graph_milestone_24900 (batch24900), and gj_continuum_wave_24900
 * (batch24899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (24891–24900): host soft probes
 * (cgj_soft_milestone_24900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=24900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24898. */
const char __libcgj_batch24898_marker[] = "libcgj-batch24898";

/* Soft milestone tag for wave 24900 (product identity = 24900). */
#define B24898_MILESTONE_TAG  24900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24898_tag(void)
{
	return B24898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_24900 - report soft milestone product tag for wave 24900.
 *
 * Always returns 24900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 24900.
 */
uint32_t
gj_milestone_tag_24900(void)
{
	(void)NULL;
	return b24898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_24900(void)
    __attribute__((alias("gj_milestone_tag_24900")));
