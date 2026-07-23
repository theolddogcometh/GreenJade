/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15898: milestone 15900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_15900(void);
 *     - Returns the milestone tag for the milestone 15900 continuum
 *       (always 15900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_15900  (alias)
 *   __libcgj_batch15898_marker = "libcgj-batch15898"
 *
 * Milestone 15900 exclusive continuum CREATE-ONLY (15891-15900). Unique
 * gj_milestone_tag_15900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_15800 / gj_milestone_tag_15700 / gj_milestone_tag_15600,
 * gj_graph_milestone_15900 (batch15900), and gj_continuum_wave_15900
 * (batch15899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15891–15900): host soft probes
 * (cgj_soft_milestone_15900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15898. */
const char __libcgj_batch15898_marker[] = "libcgj-batch15898";

/* Soft milestone tag for wave 15900 (product identity = 15900). */
#define B15898_MILESTONE_TAG  15900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15898_tag(void)
{
	return B15898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_15900 - report soft milestone product tag for wave 15900.
 *
 * Always returns 15900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 15900.
 */
uint32_t
gj_milestone_tag_15900(void)
{
	(void)NULL;
	return b15898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_15900(void)
    __attribute__((alias("gj_milestone_tag_15900")));
