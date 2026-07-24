/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch70298: milestone 70300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_70300(void);
 *     - Returns the milestone tag for the milestone 70300 continuum
 *       (always 70300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_70300  (alias)
 *   __libcgj_batch70298_marker = "libcgj-batch70298"
 *
 * Milestone 70300 exclusive continuum CREATE-ONLY (25891-70300). Unique
 * gj_milestone_tag_70300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_70300 (batch70300), and gj_continuum_wave_70300
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–70300): host soft probes
 * (cgj_soft_milestone_70300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=70300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 70298. */
const char __libcgj_batch70298_marker[] = "libcgj-batch70298";

/* Soft milestone tag for wave 70300 (product identity = 70300). */
#define B70298_MILESTONE_TAG  70300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b70298_tag(void)
{
	return B70298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_70300 - report soft milestone product tag for wave 70300.
 *
 * Always returns 70300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 70300.
 */
uint32_t
gj_milestone_tag_70300(void)
{
	(void)NULL;
	return b70298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_70300(void)
    __attribute__((alias("gj_milestone_tag_70300")));
