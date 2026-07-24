/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24498: milestone 24500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_24500(void);
 *     - Returns the milestone tag for the milestone 24500 continuum
 *       (always 24500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_24500  (alias)
 *   __libcgj_batch24498_marker = "libcgj-batch24498"
 *
 * Milestone 24500 exclusive continuum CREATE-ONLY (24491-24500). Unique
 * gj_milestone_tag_24500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_24400 / gj_milestone_tag_24300 / gj_milestone_tag_24200,
 * gj_graph_milestone_24500 (batch24500), and gj_continuum_wave_24500
 * (batch24499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (24491–24500): host soft probes
 * (cgj_soft_milestone_24500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=24500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24498. */
const char __libcgj_batch24498_marker[] = "libcgj-batch24498";

/* Soft milestone tag for wave 24500 (product identity = 24500). */
#define B24498_MILESTONE_TAG  24500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24498_tag(void)
{
	return B24498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_24500 - report soft milestone product tag for wave 24500.
 *
 * Always returns 24500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 24500.
 */
uint32_t
gj_milestone_tag_24500(void)
{
	(void)NULL;
	return b24498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_24500(void)
    __attribute__((alias("gj_milestone_tag_24500")));
