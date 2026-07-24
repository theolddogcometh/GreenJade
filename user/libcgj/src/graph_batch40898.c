/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40898: milestone 40900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_40900(void);
 *     - Returns the milestone tag for the milestone 40900 continuum
 *       (always 40900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_40900  (alias)
 *   __libcgj_batch40898_marker = "libcgj-batch40898"
 *
 * Milestone 40900 exclusive continuum CREATE-ONLY (40891-40900). Unique
 * gj_milestone_tag_40900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_40800 / gj_milestone_tag_40700 / gj_milestone_tag_40600,
 * gj_graph_milestone_40900 (batch40900), and gj_continuum_wave_40900
 * (batch40899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40891–40900): host soft probes
 * (cgj_soft_milestone_40900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=40900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40898. */
const char __libcgj_batch40898_marker[] = "libcgj-batch40898";

/* Soft milestone tag for wave 40900 (product identity = 40900). */
#define B40898_MILESTONE_TAG  40900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40898_tag(void)
{
	return B40898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_40900 - report soft milestone product tag for wave 40900.
 *
 * Always returns 40900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 40900.
 */
uint32_t
gj_milestone_tag_40900(void)
{
	(void)NULL;
	return b40898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_40900(void)
    __attribute__((alias("gj_milestone_tag_40900")));
