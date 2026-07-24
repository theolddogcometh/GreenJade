/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25398: milestone 25400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_25400(void);
 *     - Returns the milestone tag for the milestone 25400 continuum
 *       (always 25400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_25400  (alias)
 *   __libcgj_batch25398_marker = "libcgj-batch25398"
 *
 * Milestone 25400 exclusive continuum CREATE-ONLY (25391-25400). Unique
 * gj_milestone_tag_25400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25300 / gj_milestone_tag_25200 / gj_milestone_tag_25100,
 * gj_graph_milestone_25400 (batch25400), and gj_continuum_wave_25400
 * (batch25399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25391–25400): host soft probes
 * (cgj_soft_milestone_25400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25398. */
const char __libcgj_batch25398_marker[] = "libcgj-batch25398";

/* Soft milestone tag for wave 25400 (product identity = 25400). */
#define B25398_MILESTONE_TAG  25400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25398_tag(void)
{
	return B25398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_25400 - report soft milestone product tag for wave 25400.
 *
 * Always returns 25400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 25400.
 */
uint32_t
gj_milestone_tag_25400(void)
{
	(void)NULL;
	return b25398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_25400(void)
    __attribute__((alias("gj_milestone_tag_25400")));
