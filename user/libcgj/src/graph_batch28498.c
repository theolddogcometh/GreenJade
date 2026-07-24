/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28498: milestone 28500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_28500(void);
 *     - Returns the milestone tag for the milestone 28500 continuum
 *       (always 28500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_28500  (alias)
 *   __libcgj_batch28498_marker = "libcgj-batch28498"
 *
 * Milestone 28500 exclusive continuum CREATE-ONLY (28491-28500). Unique
 * gj_milestone_tag_28500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_28400 / gj_milestone_tag_28300 / gj_milestone_tag_28200,
 * gj_graph_milestone_28500 (batch28500), and gj_continuum_wave_28500
 * (batch28499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28491–28500): host soft probes
 * (cgj_soft_milestone_28500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28498. */
const char __libcgj_batch28498_marker[] = "libcgj-batch28498";

/* Soft milestone tag for wave 28500 (product identity = 28500). */
#define B28498_MILESTONE_TAG  28500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28498_tag(void)
{
	return B28498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_28500 - report soft milestone product tag for wave 28500.
 *
 * Always returns 28500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 28500.
 */
uint32_t
gj_milestone_tag_28500(void)
{
	(void)NULL;
	return b28498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_28500(void)
    __attribute__((alias("gj_milestone_tag_28500")));
