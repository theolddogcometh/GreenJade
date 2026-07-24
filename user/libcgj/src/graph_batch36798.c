/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36798: milestone 36800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_36800(void);
 *     - Returns the milestone tag for the milestone 36800 continuum
 *       (always 36800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_36800  (alias)
 *   __libcgj_batch36798_marker = "libcgj-batch36798"
 *
 * Milestone 36800 exclusive continuum CREATE-ONLY (36791-36800). Unique
 * gj_milestone_tag_36800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_36700 / gj_milestone_tag_36600 / gj_milestone_tag_36500,
 * gj_graph_milestone_36800 (batch36800), and gj_continuum_wave_36800
 * (batch36799).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36791–36800): host soft probes
 * (cgj_soft_milestone_36800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36798. */
const char __libcgj_batch36798_marker[] = "libcgj-batch36798";

/* Soft milestone tag for wave 36800 (product identity = 36800). */
#define B36798_MILESTONE_TAG  36800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36798_tag(void)
{
	return B36798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_36800 - report soft milestone product tag for wave 36800.
 *
 * Always returns 36800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 36800.
 */
uint32_t
gj_milestone_tag_36800(void)
{
	(void)NULL;
	return b36798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_36800(void)
    __attribute__((alias("gj_milestone_tag_36800")));
