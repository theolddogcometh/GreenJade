/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35398: milestone 35400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_35400(void);
 *     - Returns the milestone tag for the milestone 35400 continuum
 *       (always 35400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_35400  (alias)
 *   __libcgj_batch35398_marker = "libcgj-batch35398"
 *
 * Milestone 35400 exclusive continuum CREATE-ONLY (35391-35400). Unique
 * gj_milestone_tag_35400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_35300 / gj_milestone_tag_35200 / gj_milestone_tag_35100,
 * gj_graph_milestone_35400 (batch35400), and gj_continuum_wave_35400
 * (batch35399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (35391–35400): host soft probes
 * (cgj_soft_milestone_35400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=35400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 35398. */
const char __libcgj_batch35398_marker[] = "libcgj-batch35398";

/* Soft milestone tag for wave 35400 (product identity = 35400). */
#define B35398_MILESTONE_TAG  35400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35398_tag(void)
{
	return B35398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_35400 - report soft milestone product tag for wave 35400.
 *
 * Always returns 35400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 35400.
 */
uint32_t
gj_milestone_tag_35400(void)
{
	(void)NULL;
	return b35398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_35400(void)
    __attribute__((alias("gj_milestone_tag_35400")));
