/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch55498: milestone 55500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_55500(void);
 *     - Returns the milestone tag for the milestone 55500 continuum
 *       (always 55500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_55500  (alias)
 *   __libcgj_batch55498_marker = "libcgj-batch55498"
 *
 * Milestone 55500 exclusive continuum CREATE-ONLY (25891-55500). Unique
 * gj_milestone_tag_55500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_55500 (batch55500), and gj_continuum_wave_55500
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–55500): host soft probes
 * (cgj_soft_milestone_55500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=55500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 55498. */
const char __libcgj_batch55498_marker[] = "libcgj-batch55498";

/* Soft milestone tag for wave 55500 (product identity = 55500). */
#define B55498_MILESTONE_TAG  55500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b55498_tag(void)
{
	return B55498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_55500 - report soft milestone product tag for wave 55500.
 *
 * Always returns 55500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 55500.
 */
uint32_t
gj_milestone_tag_55500(void)
{
	(void)NULL;
	return b55498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_55500(void)
    __attribute__((alias("gj_milestone_tag_55500")));
