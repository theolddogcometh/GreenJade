/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35498: milestone 35500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_35500(void);
 *     - Returns the milestone tag for the milestone 35500 continuum
 *       (always 35500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_35500  (alias)
 *   __libcgj_batch35498_marker = "libcgj-batch35498"
 *
 * Milestone 35500 exclusive continuum CREATE-ONLY (35491-35500). Unique
 * gj_milestone_tag_35500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_35400 / gj_milestone_tag_35300 / gj_milestone_tag_35200,
 * gj_graph_milestone_35500 (batch35500), and gj_continuum_wave_35500
 * (batch35499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (35491–35500): host soft probes
 * (cgj_soft_milestone_35500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=35500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 35498. */
const char __libcgj_batch35498_marker[] = "libcgj-batch35498";

/* Soft milestone tag for wave 35500 (product identity = 35500). */
#define B35498_MILESTONE_TAG  35500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35498_tag(void)
{
	return B35498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_35500 - report soft milestone product tag for wave 35500.
 *
 * Always returns 35500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 35500.
 */
uint32_t
gj_milestone_tag_35500(void)
{
	(void)NULL;
	return b35498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_35500(void)
    __attribute__((alias("gj_milestone_tag_35500")));
