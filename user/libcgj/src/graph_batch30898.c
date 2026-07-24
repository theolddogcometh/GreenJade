/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30898: milestone 30900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_30900(void);
 *     - Returns the milestone tag for the milestone 30900 continuum
 *       (always 30900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_30900  (alias)
 *   __libcgj_batch30898_marker = "libcgj-batch30898"
 *
 * Milestone 30900 exclusive continuum CREATE-ONLY (30891-30900). Unique
 * gj_milestone_tag_30900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_30800 / gj_milestone_tag_30700 / gj_milestone_tag_30600,
 * gj_graph_milestone_30900 (batch30900), and gj_continuum_wave_30900
 * (batch30899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30891–30900): host soft probes
 * (cgj_soft_milestone_30900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30898. */
const char __libcgj_batch30898_marker[] = "libcgj-batch30898";

/* Soft milestone tag for wave 30900 (product identity = 30900). */
#define B30898_MILESTONE_TAG  30900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30898_tag(void)
{
	return B30898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_30900 - report soft milestone product tag for wave 30900.
 *
 * Always returns 30900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 30900.
 */
uint32_t
gj_milestone_tag_30900(void)
{
	(void)NULL;
	return b30898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_30900(void)
    __attribute__((alias("gj_milestone_tag_30900")));
