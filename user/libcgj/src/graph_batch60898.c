/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch60898: milestone 60900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_60900(void);
 *     - Returns the milestone tag for the milestone 60900 continuum
 *       (always 60900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_60900  (alias)
 *   __libcgj_batch60898_marker = "libcgj-batch60898"
 *
 * Milestone 60900 exclusive continuum CREATE-ONLY (25891-60900). Unique
 * gj_milestone_tag_60900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_60900 (batch60900), and gj_continuum_wave_60900
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–60900): host soft probes
 * (cgj_soft_milestone_60900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=60900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 60898. */
const char __libcgj_batch60898_marker[] = "libcgj-batch60898";

/* Soft milestone tag for wave 60900 (product identity = 60900). */
#define B60898_MILESTONE_TAG  60900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b60898_tag(void)
{
	return B60898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_60900 - report soft milestone product tag for wave 60900.
 *
 * Always returns 60900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 60900.
 */
uint32_t
gj_milestone_tag_60900(void)
{
	(void)NULL;
	return b60898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_60900(void)
    __attribute__((alias("gj_milestone_tag_60900")));
