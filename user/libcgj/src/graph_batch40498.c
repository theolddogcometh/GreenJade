/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40498: milestone 40500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_40500(void);
 *     - Returns the milestone tag for the milestone 40500 continuum
 *       (always 40500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_40500  (alias)
 *   __libcgj_batch40498_marker = "libcgj-batch40498"
 *
 * Milestone 40500 exclusive continuum CREATE-ONLY (40491-40500). Unique
 * gj_milestone_tag_40500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_40400 / gj_milestone_tag_40300 / gj_milestone_tag_40200,
 * gj_graph_milestone_40500 (batch40500), and gj_continuum_wave_40500
 * (batch40499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40491–40500): host soft probes
 * (cgj_soft_milestone_40500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=40500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40498. */
const char __libcgj_batch40498_marker[] = "libcgj-batch40498";

/* Soft milestone tag for wave 40500 (product identity = 40500). */
#define B40498_MILESTONE_TAG  40500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40498_tag(void)
{
	return B40498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_40500 - report soft milestone product tag for wave 40500.
 *
 * Always returns 40500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 40500.
 */
uint32_t
gj_milestone_tag_40500(void)
{
	(void)NULL;
	return b40498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_40500(void)
    __attribute__((alias("gj_milestone_tag_40500")));
