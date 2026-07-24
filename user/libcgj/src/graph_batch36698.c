/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36698: milestone 36700 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_36700(void);
 *     - Returns the milestone tag for the milestone 36700 continuum
 *       (always 36700u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_36700  (alias)
 *   __libcgj_batch36698_marker = "libcgj-batch36698"
 *
 * Milestone 36700 exclusive continuum CREATE-ONLY (36691-36700). Unique
 * gj_milestone_tag_36700 surface only; no multi-def. Distinct from
 * gj_milestone_tag_36600 / gj_milestone_tag_36500 / gj_milestone_tag_36400,
 * gj_graph_milestone_36700 (batch36700), and gj_continuum_wave_36700
 * (batch36699).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36691–36700): host soft probes
 * (cgj_soft_milestone_36700.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36700 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36698. */
const char __libcgj_batch36698_marker[] = "libcgj-batch36698";

/* Soft milestone tag for wave 36700 (product identity = 36700). */
#define B36698_MILESTONE_TAG  36700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36698_tag(void)
{
	return B36698_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_36700 - report soft milestone product tag for wave 36700.
 *
 * Always returns 36700u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 36700.
 */
uint32_t
gj_milestone_tag_36700(void)
{
	(void)NULL;
	return b36698_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_36700(void)
    __attribute__((alias("gj_milestone_tag_36700")));
