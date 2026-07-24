/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21798: milestone 21800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_21800(void);
 *     - Returns the milestone tag for the milestone 21800 continuum
 *       (always 21800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_21800  (alias)
 *   __libcgj_batch21798_marker = "libcgj-batch21798"
 *
 * Milestone 21800 exclusive continuum CREATE-ONLY (21791-21800). Unique
 * gj_milestone_tag_21800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_21700 / gj_milestone_tag_21600 / gj_milestone_tag_21500,
 * gj_graph_milestone_21800 (batch21800), and gj_continuum_wave_21800
 * (batch21799).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21791–21800): host soft probes
 * (cgj_soft_milestone_21800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21798. */
const char __libcgj_batch21798_marker[] = "libcgj-batch21798";

/* Soft milestone tag for wave 21800 (product identity = 21800). */
#define B21798_MILESTONE_TAG  21800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21798_tag(void)
{
	return B21798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_21800 - report soft milestone product tag for wave 21800.
 *
 * Always returns 21800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 21800.
 */
uint32_t
gj_milestone_tag_21800(void)
{
	(void)NULL;
	return b21798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_21800(void)
    __attribute__((alias("gj_milestone_tag_21800")));
