/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29798: milestone 29800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_29800(void);
 *     - Returns the milestone tag for the milestone 29800 continuum
 *       (always 29800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_29800  (alias)
 *   __libcgj_batch29798_marker = "libcgj-batch29798"
 *
 * Milestone 29800 exclusive continuum CREATE-ONLY (29791-29800). Unique
 * gj_milestone_tag_29800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_29700 / gj_milestone_tag_29600 / gj_milestone_tag_29500,
 * gj_graph_milestone_29800 (batch29800), and gj_continuum_wave_29800
 * (batch29799).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (29791–29800): host soft probes
 * (cgj_soft_milestone_29800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=29800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 29798. */
const char __libcgj_batch29798_marker[] = "libcgj-batch29798";

/* Soft milestone tag for wave 29800 (product identity = 29800). */
#define B29798_MILESTONE_TAG  29800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29798_tag(void)
{
	return B29798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_29800 - report soft milestone product tag for wave 29800.
 *
 * Always returns 29800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 29800.
 */
uint32_t
gj_milestone_tag_29800(void)
{
	(void)NULL;
	return b29798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_29800(void)
    __attribute__((alias("gj_milestone_tag_29800")));
