/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30798: milestone 30800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_30800(void);
 *     - Returns the milestone tag for the milestone 30800 continuum
 *       (always 30800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_30800  (alias)
 *   __libcgj_batch30798_marker = "libcgj-batch30798"
 *
 * Milestone 30800 exclusive continuum CREATE-ONLY (30791-30800). Unique
 * gj_milestone_tag_30800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_30700 / gj_milestone_tag_30600 / gj_milestone_tag_30500,
 * gj_graph_milestone_30800 (batch30800), and gj_continuum_wave_30800
 * (batch30799).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30791–30800): host soft probes
 * (cgj_soft_milestone_30800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30798. */
const char __libcgj_batch30798_marker[] = "libcgj-batch30798";

/* Soft milestone tag for wave 30800 (product identity = 30800). */
#define B30798_MILESTONE_TAG  30800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30798_tag(void)
{
	return B30798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_30800 - report soft milestone product tag for wave 30800.
 *
 * Always returns 30800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 30800.
 */
uint32_t
gj_milestone_tag_30800(void)
{
	(void)NULL;
	return b30798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_30800(void)
    __attribute__((alias("gj_milestone_tag_30800")));
