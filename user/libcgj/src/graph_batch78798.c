/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch78798: milestone 78800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_78800(void);
 *     - Returns the milestone tag for the milestone 78800 continuum
 *       (always 78800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_78800  (alias)
 *   __libcgj_batch78798_marker = "libcgj-batch78798"
 *
 * Milestone 78800 exclusive continuum CREATE-ONLY (25891-78800). Unique
 * gj_milestone_tag_78800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_78800 (batch78800), and gj_continuum_wave_78800
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–78800): host soft probes
 * (cgj_soft_milestone_78800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=78800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 78798. */
const char __libcgj_batch78798_marker[] = "libcgj-batch78798";

/* Soft milestone tag for wave 78800 (product identity = 78800). */
#define B78798_MILESTONE_TAG  78800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b78798_tag(void)
{
	return B78798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_78800 - report soft milestone product tag for wave 78800.
 *
 * Always returns 78800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 78800.
 */
uint32_t
gj_milestone_tag_78800(void)
{
	(void)NULL;
	return b78798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_78800(void)
    __attribute__((alias("gj_milestone_tag_78800")));
