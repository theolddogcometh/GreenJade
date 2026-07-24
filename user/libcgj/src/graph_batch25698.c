/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25698: milestone 25700 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_25700(void);
 *     - Returns the milestone tag for the milestone 25700 continuum
 *       (always 25700u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_25700  (alias)
 *   __libcgj_batch25698_marker = "libcgj-batch25698"
 *
 * Milestone 25700 exclusive continuum CREATE-ONLY (25691-25700). Unique
 * gj_milestone_tag_25700 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25600 / gj_milestone_tag_25500 / gj_milestone_tag_25400,
 * gj_graph_milestone_25700 (batch25700), and gj_continuum_wave_25700
 * (batch25699).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25691–25700): host soft probes
 * (cgj_soft_milestone_25700.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25700 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25698. */
const char __libcgj_batch25698_marker[] = "libcgj-batch25698";

/* Soft milestone tag for wave 25700 (product identity = 25700). */
#define B25698_MILESTONE_TAG  25700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25698_tag(void)
{
	return B25698_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_25700 - report soft milestone product tag for wave 25700.
 *
 * Always returns 25700u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 25700.
 */
uint32_t
gj_milestone_tag_25700(void)
{
	(void)NULL;
	return b25698_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_25700(void)
    __attribute__((alias("gj_milestone_tag_25700")));
