/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16498: milestone 16500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_16500(void);
 *     - Returns the milestone tag for the milestone 16500 continuum
 *       (always 16500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_16500  (alias)
 *   __libcgj_batch16498_marker = "libcgj-batch16498"
 *
 * Milestone 16500 exclusive continuum CREATE-ONLY (16491-16500). Unique
 * gj_milestone_tag_16500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_16500 / gj_milestone_tag_16300 / gj_milestone_tag_16200,
 * gj_graph_milestone_16500 (batch16500), and gj_continuum_wave_16500
 * (batch16499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (16491–16500): host soft probes
 * (cgj_soft_milestone_16500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=16500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16498. */
const char __libcgj_batch16498_marker[] = "libcgj-batch16498";

/* Soft milestone tag for wave 16500 (product identity = 16500). */
#define B16498_MILESTONE_TAG  16500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16498_tag(void)
{
	return B16498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_16500 - report soft milestone product tag for wave 16500.
 *
 * Always returns 16500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 16500.
 */
uint32_t
gj_milestone_tag_16500(void)
{
	(void)NULL;
	return b16498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_16500(void)
    __attribute__((alias("gj_milestone_tag_16500")));
