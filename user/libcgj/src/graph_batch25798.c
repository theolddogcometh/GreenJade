/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25798: milestone 25800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_25800(void);
 *     - Returns the milestone tag for the milestone 25800 continuum
 *       (always 25800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_25800  (alias)
 *   __libcgj_batch25798_marker = "libcgj-batch25798"
 *
 * Milestone 25800 exclusive continuum CREATE-ONLY (25791-25800). Unique
 * gj_milestone_tag_25800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25700 / gj_milestone_tag_25600 / gj_milestone_tag_25500,
 * gj_graph_milestone_25800 (batch25800), and gj_continuum_wave_25800
 * (batch25799).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25791–25800): host soft probes
 * (cgj_soft_milestone_25800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25798. */
const char __libcgj_batch25798_marker[] = "libcgj-batch25798";

/* Soft milestone tag for wave 25800 (product identity = 25800). */
#define B25798_MILESTONE_TAG  25800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25798_tag(void)
{
	return B25798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_25800 - report soft milestone product tag for wave 25800.
 *
 * Always returns 25800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 25800.
 */
uint32_t
gj_milestone_tag_25800(void)
{
	(void)NULL;
	return b25798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_25800(void)
    __attribute__((alias("gj_milestone_tag_25800")));
