/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22898: milestone 22900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_22900(void);
 *     - Returns the milestone tag for the milestone 22900 continuum
 *       (always 22900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_22900  (alias)
 *   __libcgj_batch22898_marker = "libcgj-batch22898"
 *
 * Milestone 22900 exclusive continuum CREATE-ONLY (22891-22900). Unique
 * gj_milestone_tag_22900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_22800 / gj_milestone_tag_22700 / gj_milestone_tag_22600,
 * gj_graph_milestone_22900 (batch22900), and gj_continuum_wave_22900
 * (batch22899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (22891–22900): host soft probes
 * (cgj_soft_milestone_22900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=22900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 22898. */
const char __libcgj_batch22898_marker[] = "libcgj-batch22898";

/* Soft milestone tag for wave 22900 (product identity = 22900). */
#define B22898_MILESTONE_TAG  22900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22898_tag(void)
{
	return B22898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_22900 - report soft milestone product tag for wave 22900.
 *
 * Always returns 22900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 22900.
 */
uint32_t
gj_milestone_tag_22900(void)
{
	(void)NULL;
	return b22898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_22900(void)
    __attribute__((alias("gj_milestone_tag_22900")));
