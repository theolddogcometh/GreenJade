/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch90898: milestone 90900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_90900(void);
 *     - Returns the milestone tag for the milestone 90900 continuum
 *       (always 90900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_90900  (alias)
 *   __libcgj_batch90898_marker = "libcgj-batch90898"
 *
 * Milestone 90900 exclusive continuum CREATE-ONLY (25891-90900). Unique
 * gj_milestone_tag_90900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_90900 (batch90900), and gj_continuum_wave_90900
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–90900): host soft probes
 * (cgj_soft_milestone_90900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=90900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 90898. */
const char __libcgj_batch90898_marker[] = "libcgj-batch90898";

/* Soft milestone tag for wave 90900 (product identity = 90900). */
#define B90898_MILESTONE_TAG  90900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b90898_tag(void)
{
	return B90898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_90900 - report soft milestone product tag for wave 90900.
 *
 * Always returns 90900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 90900.
 */
uint32_t
gj_milestone_tag_90900(void)
{
	(void)NULL;
	return b90898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_90900(void)
    __attribute__((alias("gj_milestone_tag_90900")));
