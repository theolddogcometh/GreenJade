/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25898: milestone 25900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_25900(void);
 *     - Returns the milestone tag for the milestone 25900 continuum
 *       (always 25900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_25900  (alias)
 *   __libcgj_batch25898_marker = "libcgj-batch25898"
 *
 * Milestone 25900 exclusive continuum CREATE-ONLY (25891-25900). Unique
 * gj_milestone_tag_25900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_25900 (batch25900), and gj_continuum_wave_25900
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–25900): host soft probes
 * (cgj_soft_milestone_25900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25898. */
const char __libcgj_batch25898_marker[] = "libcgj-batch25898";

/* Soft milestone tag for wave 25900 (product identity = 25900). */
#define B25898_MILESTONE_TAG  25900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25898_tag(void)
{
	return B25898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_25900 - report soft milestone product tag for wave 25900.
 *
 * Always returns 25900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 25900.
 */
uint32_t
gj_milestone_tag_25900(void)
{
	(void)NULL;
	return b25898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_25900(void)
    __attribute__((alias("gj_milestone_tag_25900")));
