/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15498: milestone 15500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_15500(void);
 *     - Returns the milestone tag for the milestone 15500 continuum
 *       (always 15500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_15500  (alias)
 *   __libcgj_batch15498_marker = "libcgj-batch15498"
 *
 * Milestone 15500 exclusive continuum CREATE-ONLY (15491-15500). Unique
 * gj_milestone_tag_15500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_15400 / gj_milestone_tag_15300 / gj_milestone_tag_15200,
 * gj_graph_milestone_15500 (batch15500), and gj_continuum_wave_15500
 * (batch15499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15491–15500): host soft probes
 * (cgj_soft_milestone_15500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15498. */
const char __libcgj_batch15498_marker[] = "libcgj-batch15498";

/* Soft milestone tag for wave 15500 (product identity = 15500). */
#define B15498_MILESTONE_TAG  15500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15498_tag(void)
{
	return B15498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_15500 - report soft milestone product tag for wave 15500.
 *
 * Always returns 15500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 15500.
 */
uint32_t
gj_milestone_tag_15500(void)
{
	(void)NULL;
	return b15498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_15500(void)
    __attribute__((alias("gj_milestone_tag_15500")));
