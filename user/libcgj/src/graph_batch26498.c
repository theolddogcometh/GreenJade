/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26498: milestone 26500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_26500(void);
 *     - Returns the milestone tag for the milestone 26500 continuum
 *       (always 26500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_26500  (alias)
 *   __libcgj_batch26498_marker = "libcgj-batch26498"
 *
 * Milestone 26500 exclusive continuum CREATE-ONLY (26491-26500). Unique
 * gj_milestone_tag_26500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_26400 / gj_milestone_tag_26300 / gj_milestone_tag_26200,
 * gj_graph_milestone_26500 (batch26500), and gj_continuum_wave_26500
 * (batch26499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26491–26500): host soft probes
 * (cgj_soft_milestone_26500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26498. */
const char __libcgj_batch26498_marker[] = "libcgj-batch26498";

/* Soft milestone tag for wave 26500 (product identity = 26500). */
#define B26498_MILESTONE_TAG  26500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26498_tag(void)
{
	return B26498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_26500 - report soft milestone product tag for wave 26500.
 *
 * Always returns 26500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 26500.
 */
uint32_t
gj_milestone_tag_26500(void)
{
	(void)NULL;
	return b26498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_26500(void)
    __attribute__((alias("gj_milestone_tag_26500")));
