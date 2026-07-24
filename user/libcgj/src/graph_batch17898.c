/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17898: milestone 17900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_17900(void);
 *     - Returns the milestone tag for the milestone 17900 continuum
 *       (always 17900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_17900  (alias)
 *   __libcgj_batch17898_marker = "libcgj-batch17898"
 *
 * Milestone 17900 exclusive continuum CREATE-ONLY (17891-17900). Unique
 * gj_milestone_tag_17900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_17700 / gj_milestone_tag_17600 / gj_milestone_tag_17500,
 * gj_graph_milestone_17900 (batch17900), and gj_continuum_wave_17900
 * (batch17899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (17891–17900): host soft probes
 * (cgj_soft_milestone_17900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=17900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 17898. */
const char __libcgj_batch17898_marker[] = "libcgj-batch17898";

/* Soft milestone tag for wave 17900 (product identity = 17900). */
#define B17898_MILESTONE_TAG  17900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17898_tag(void)
{
	return B17898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_17900 - report soft milestone product tag for wave 17900.
 *
 * Always returns 17900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 17900.
 */
uint32_t
gj_milestone_tag_17900(void)
{
	(void)NULL;
	return b17898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_17900(void)
    __attribute__((alias("gj_milestone_tag_17900")));
