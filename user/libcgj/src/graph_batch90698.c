/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch90698: milestone 90700 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_90700(void);
 *     - Returns the milestone tag for the milestone 90700 continuum
 *       (always 90700u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_90700  (alias)
 *   __libcgj_batch90698_marker = "libcgj-batch90698"
 *
 * Milestone 90700 exclusive continuum CREATE-ONLY (25891-90700). Unique
 * gj_milestone_tag_90700 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_90700 (batch90700), and gj_continuum_wave_90700
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–90700): host soft probes
 * (cgj_soft_milestone_90700.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=90700 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 90698. */
const char __libcgj_batch90698_marker[] = "libcgj-batch90698";

/* Soft milestone tag for wave 90700 (product identity = 90700). */
#define B90698_MILESTONE_TAG  90700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b90698_tag(void)
{
	return B90698_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_90700 - report soft milestone product tag for wave 90700.
 *
 * Always returns 90700u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 90700.
 */
uint32_t
gj_milestone_tag_90700(void)
{
	(void)NULL;
	return b90698_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_90700(void)
    __attribute__((alias("gj_milestone_tag_90700")));
