/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95598: milestone 95600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_95600(void);
 *     - Returns the milestone tag for the milestone 95600 continuum
 *       (always 95600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_95600  (alias)
 *   __libcgj_batch95598_marker = "libcgj-batch95598"
 *
 * Milestone 95600 exclusive continuum CREATE-ONLY (25891-95600). Unique
 * gj_milestone_tag_95600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_95600 (batch95600), and gj_continuum_wave_95600
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–95600): host soft probes
 * (cgj_soft_milestone_95600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=95600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 95598. */
const char __libcgj_batch95598_marker[] = "libcgj-batch95598";

/* Soft milestone tag for wave 95600 (product identity = 95600). */
#define B95598_MILESTONE_TAG  95600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b95598_tag(void)
{
	return B95598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_95600 - report soft milestone product tag for wave 95600.
 *
 * Always returns 95600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 95600.
 */
uint32_t
gj_milestone_tag_95600(void)
{
	(void)NULL;
	return b95598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_95600(void)
    __attribute__((alias("gj_milestone_tag_95600")));
