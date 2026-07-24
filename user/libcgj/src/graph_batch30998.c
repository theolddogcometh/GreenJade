/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30998: milestone 31000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_31000(void);
 *     - Returns the milestone tag for the milestone 31000 continuum
 *       (always 31000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_31000  (alias)
 *   __libcgj_batch30998_marker = "libcgj-batch30998"
 *
 * Milestone 31000 exclusive continuum CREATE-ONLY (30991-31000). Unique
 * gj_milestone_tag_31000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_30900 / gj_milestone_tag_30800 / gj_milestone_tag_30700,
 * gj_graph_milestone_31000 (batch31000), and gj_continuum_wave_31000
 * (batch30999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30991–31000): host soft probes
 * (cgj_soft_milestone_31000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=31000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30998. */
const char __libcgj_batch30998_marker[] = "libcgj-batch30998";

/* Soft milestone tag for wave 31000 (product identity = 31000). */
#define B30998_MILESTONE_TAG  31000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30998_tag(void)
{
	return B30998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_31000 - report soft milestone product tag for wave 31000.
 *
 * Always returns 31000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 31000.
 */
uint32_t
gj_milestone_tag_31000(void)
{
	(void)NULL;
	return b30998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_31000(void)
    __attribute__((alias("gj_milestone_tag_31000")));
