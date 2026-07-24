/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18898: milestone 18900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_18900(void);
 *     - Returns the milestone tag for the milestone 18900 continuum
 *       (always 18900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_18900  (alias)
 *   __libcgj_batch18898_marker = "libcgj-batch18898"
 *
 * Milestone 18900 exclusive continuum CREATE-ONLY (18891-18900). Unique
 * gj_milestone_tag_18900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_18700 / gj_milestone_tag_18600 / gj_milestone_tag_18500,
 * gj_graph_milestone_18900 (batch18900), and gj_continuum_wave_18900
 * (batch18899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18891–18900): host soft probes
 * (cgj_soft_milestone_18900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=18900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18898. */
const char __libcgj_batch18898_marker[] = "libcgj-batch18898";

/* Soft milestone tag for wave 18900 (product identity = 18900). */
#define B18898_MILESTONE_TAG  18900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18898_tag(void)
{
	return B18898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_18900 - report soft milestone product tag for wave 18900.
 *
 * Always returns 18900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 18900.
 */
uint32_t
gj_milestone_tag_18900(void)
{
	(void)NULL;
	return b18898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_18900(void)
    __attribute__((alias("gj_milestone_tag_18900")));
