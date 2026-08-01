/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch92898: milestone 92900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_92900(void);
 *     - Returns the milestone tag for the milestone 92900 continuum
 *       (always 92900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_92900  (alias)
 *   __libcgj_batch92898_marker = "libcgj-batch92898"
 *
 * Milestone 92900 exclusive continuum CREATE-ONLY (25891-92900). Unique
 * gj_milestone_tag_92900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_92900 (batch92900), and gj_continuum_wave_92900
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–92900): host soft probes
 * (cgj_soft_milestone_92900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=92900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 92898. */
const char __libcgj_batch92898_marker[] = "libcgj-batch92898";

/* Soft milestone tag for wave 92900 (product identity = 92900). */
#define B92898_MILESTONE_TAG  92900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b92898_tag(void)
{
	return B92898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_92900 - report soft milestone product tag for wave 92900.
 *
 * Always returns 92900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 92900.
 */
uint32_t
gj_milestone_tag_92900(void)
{
	(void)NULL;
	return b92898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_92900(void)
    __attribute__((alias("gj_milestone_tag_92900")));
