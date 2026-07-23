/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15698: milestone 15700 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_15700(void);
 *     - Returns the milestone tag for the milestone 15700 continuum
 *       (always 15700u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_15700  (alias)
 *   __libcgj_batch15698_marker = "libcgj-batch15698"
 *
 * Milestone 15700 exclusive continuum CREATE-ONLY (15691-15700). Unique
 * gj_milestone_tag_15700 surface only; no multi-def. Distinct from
 * gj_milestone_tag_15600 / gj_milestone_tag_15500 / gj_milestone_tag_15400,
 * gj_graph_milestone_15700 (batch15700), and gj_continuum_wave_15700
 * (batch15699).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15691–15700): host soft probes
 * (cgj_soft_milestone_15700.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15700 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15698. */
const char __libcgj_batch15698_marker[] = "libcgj-batch15698";

/* Soft milestone tag for wave 15700 (product identity = 15700). */
#define B15698_MILESTONE_TAG  15700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15698_tag(void)
{
	return B15698_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_15700 - report soft milestone product tag for wave 15700.
 *
 * Always returns 15700u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 15700.
 */
uint32_t
gj_milestone_tag_15700(void)
{
	(void)NULL;
	return b15698_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_15700(void)
    __attribute__((alias("gj_milestone_tag_15700")));
