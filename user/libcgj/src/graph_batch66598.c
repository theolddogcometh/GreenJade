/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch66598: milestone 66600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_66600(void);
 *     - Returns the milestone tag for the milestone 66600 continuum
 *       (always 66600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_66600  (alias)
 *   __libcgj_batch66598_marker = "libcgj-batch66598"
 *
 * Milestone 66600 exclusive continuum CREATE-ONLY (25891-66600). Unique
 * gj_milestone_tag_66600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_66600 (batch66600), and gj_continuum_wave_66600
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–66600): host soft probes
 * (cgj_soft_milestone_66600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=66600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 66598. */
const char __libcgj_batch66598_marker[] = "libcgj-batch66598";

/* Soft milestone tag for wave 66600 (product identity = 66600). */
#define B66598_MILESTONE_TAG  66600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b66598_tag(void)
{
	return B66598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_66600 - report soft milestone product tag for wave 66600.
 *
 * Always returns 66600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 66600.
 */
uint32_t
gj_milestone_tag_66600(void)
{
	(void)NULL;
	return b66598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_66600(void)
    __attribute__((alias("gj_milestone_tag_66600")));
