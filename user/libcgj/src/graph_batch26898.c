/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26898: milestone 26900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_26900(void);
 *     - Returns the milestone tag for the milestone 26900 continuum
 *       (always 26900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_26900  (alias)
 *   __libcgj_batch26898_marker = "libcgj-batch26898"
 *
 * Milestone 26900 exclusive continuum CREATE-ONLY (26891-26900). Unique
 * gj_milestone_tag_26900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_26800 / gj_milestone_tag_26700 / gj_milestone_tag_26600,
 * gj_graph_milestone_26900 (batch26900), and gj_continuum_wave_26900
 * (batch26899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26891–26900): host soft probes
 * (cgj_soft_milestone_26900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26898. */
const char __libcgj_batch26898_marker[] = "libcgj-batch26898";

/* Soft milestone tag for wave 26900 (product identity = 26900). */
#define B26898_MILESTONE_TAG  26900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26898_tag(void)
{
	return B26898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_26900 - report soft milestone product tag for wave 26900.
 *
 * Always returns 26900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 26900.
 */
uint32_t
gj_milestone_tag_26900(void)
{
	(void)NULL;
	return b26898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_26900(void)
    __attribute__((alias("gj_milestone_tag_26900")));
