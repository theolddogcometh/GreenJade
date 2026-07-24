/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch90498: milestone 90500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_90500(void);
 *     - Returns the milestone tag for the milestone 90500 continuum
 *       (always 90500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_90500  (alias)
 *   __libcgj_batch90498_marker = "libcgj-batch90498"
 *
 * Milestone 90500 exclusive continuum CREATE-ONLY (25891-90500). Unique
 * gj_milestone_tag_90500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_90500 (batch90500), and gj_continuum_wave_90500
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–90500): host soft probes
 * (cgj_soft_milestone_90500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=90500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 90498. */
const char __libcgj_batch90498_marker[] = "libcgj-batch90498";

/* Soft milestone tag for wave 90500 (product identity = 90500). */
#define B90498_MILESTONE_TAG  90500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b90498_tag(void)
{
	return B90498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_90500 - report soft milestone product tag for wave 90500.
 *
 * Always returns 90500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 90500.
 */
uint32_t
gj_milestone_tag_90500(void)
{
	(void)NULL;
	return b90498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_90500(void)
    __attribute__((alias("gj_milestone_tag_90500")));
