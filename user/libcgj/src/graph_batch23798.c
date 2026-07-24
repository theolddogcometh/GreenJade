/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23798: milestone 23800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_23800(void);
 *     - Returns the milestone tag for the milestone 23800 continuum
 *       (always 23800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_23800  (alias)
 *   __libcgj_batch23798_marker = "libcgj-batch23798"
 *
 * Milestone 23800 exclusive continuum CREATE-ONLY (23791-23800). Unique
 * gj_milestone_tag_23800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_23700 / gj_milestone_tag_23600 / gj_milestone_tag_23500,
 * gj_graph_milestone_23800 (batch23800), and gj_continuum_wave_23800
 * (batch23799).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (23791–23800): host soft probes
 * (cgj_soft_milestone_23800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=23800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 23798. */
const char __libcgj_batch23798_marker[] = "libcgj-batch23798";

/* Soft milestone tag for wave 23800 (product identity = 23800). */
#define B23798_MILESTONE_TAG  23800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23798_tag(void)
{
	return B23798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_23800 - report soft milestone product tag for wave 23800.
 *
 * Always returns 23800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 23800.
 */
uint32_t
gj_milestone_tag_23800(void)
{
	(void)NULL;
	return b23798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_23800(void)
    __attribute__((alias("gj_milestone_tag_23800")));
