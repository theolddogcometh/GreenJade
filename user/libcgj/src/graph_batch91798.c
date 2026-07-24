/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch91798: milestone 91800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_91800(void);
 *     - Returns the milestone tag for the milestone 91800 continuum
 *       (always 91800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_91800  (alias)
 *   __libcgj_batch91798_marker = "libcgj-batch91798"
 *
 * Milestone 91800 exclusive continuum CREATE-ONLY (25891-91800). Unique
 * gj_milestone_tag_91800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_91800 (batch91800), and gj_continuum_wave_91800
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–91800): host soft probes
 * (cgj_soft_milestone_91800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=91800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 91798. */
const char __libcgj_batch91798_marker[] = "libcgj-batch91798";

/* Soft milestone tag for wave 91800 (product identity = 91800). */
#define B91798_MILESTONE_TAG  91800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b91798_tag(void)
{
	return B91798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_91800 - report soft milestone product tag for wave 91800.
 *
 * Always returns 91800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 91800.
 */
uint32_t
gj_milestone_tag_91800(void)
{
	(void)NULL;
	return b91798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_91800(void)
    __attribute__((alias("gj_milestone_tag_91800")));
