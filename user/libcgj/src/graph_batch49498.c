/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49498: milestone 49500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_49500(void);
 *     - Returns the milestone tag for the milestone 49500 continuum
 *       (always 49500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_49500  (alias)
 *   __libcgj_batch49498_marker = "libcgj-batch49498"
 *
 * Milestone 49500 exclusive continuum CREATE-ONLY (49491-49500). Unique
 * gj_milestone_tag_49500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_49400 / gj_milestone_tag_49300 / gj_milestone_tag_49200,
 * gj_graph_milestone_49500 (batch49500), and gj_continuum_wave_49500
 * (batch49499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (49491–49500): host soft probes
 * (cgj_soft_milestone_49500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=49500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 49498. */
const char __libcgj_batch49498_marker[] = "libcgj-batch49498";

/* Soft milestone tag for wave 49500 (product identity = 49500). */
#define B49498_MILESTONE_TAG  49500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49498_tag(void)
{
	return B49498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_49500 - report soft milestone product tag for wave 49500.
 *
 * Always returns 49500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 49500.
 */
uint32_t
gj_milestone_tag_49500(void)
{
	(void)NULL;
	return b49498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_49500(void)
    __attribute__((alias("gj_milestone_tag_49500")));
