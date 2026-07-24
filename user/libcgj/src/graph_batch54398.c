/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch54398: milestone 54400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_54400(void);
 *     - Returns the milestone tag for the milestone 54400 continuum
 *       (always 54400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_54400  (alias)
 *   __libcgj_batch54398_marker = "libcgj-batch54398"
 *
 * Milestone 54400 exclusive continuum CREATE-ONLY (25891-54400). Unique
 * gj_milestone_tag_54400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_54400 (batch54400), and gj_continuum_wave_54400
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–54400): host soft probes
 * (cgj_soft_milestone_54400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=54400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 54398. */
const char __libcgj_batch54398_marker[] = "libcgj-batch54398";

/* Soft milestone tag for wave 54400 (product identity = 54400). */
#define B54398_MILESTONE_TAG  54400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b54398_tag(void)
{
	return B54398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_54400 - report soft milestone product tag for wave 54400.
 *
 * Always returns 54400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 54400.
 */
uint32_t
gj_milestone_tag_54400(void)
{
	(void)NULL;
	return b54398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_54400(void)
    __attribute__((alias("gj_milestone_tag_54400")));
