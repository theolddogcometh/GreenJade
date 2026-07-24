/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25498: milestone 25500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_25500(void);
 *     - Returns the milestone tag for the milestone 25500 continuum
 *       (always 25500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_25500  (alias)
 *   __libcgj_batch25498_marker = "libcgj-batch25498"
 *
 * Milestone 25500 exclusive continuum CREATE-ONLY (25491-25500). Unique
 * gj_milestone_tag_25500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25400 / gj_milestone_tag_25300 / gj_milestone_tag_25200,
 * gj_graph_milestone_25500 (batch25500), and gj_continuum_wave_25500
 * (batch25499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25491–25500): host soft probes
 * (cgj_soft_milestone_25500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25498. */
const char __libcgj_batch25498_marker[] = "libcgj-batch25498";

/* Soft milestone tag for wave 25500 (product identity = 25500). */
#define B25498_MILESTONE_TAG  25500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25498_tag(void)
{
	return B25498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_25500 - report soft milestone product tag for wave 25500.
 *
 * Always returns 25500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 25500.
 */
uint32_t
gj_milestone_tag_25500(void)
{
	(void)NULL;
	return b25498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_25500(void)
    __attribute__((alias("gj_milestone_tag_25500")));
