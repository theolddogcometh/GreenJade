/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch78298: milestone 78300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_78300(void);
 *     - Returns the milestone tag for the milestone 78300 continuum
 *       (always 78300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_78300  (alias)
 *   __libcgj_batch78298_marker = "libcgj-batch78298"
 *
 * Milestone 78300 exclusive continuum CREATE-ONLY (25891-78300). Unique
 * gj_milestone_tag_78300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_78300 (batch78300), and gj_continuum_wave_78300
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–78300): host soft probes
 * (cgj_soft_milestone_78300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=78300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 78298. */
const char __libcgj_batch78298_marker[] = "libcgj-batch78298";

/* Soft milestone tag for wave 78300 (product identity = 78300). */
#define B78298_MILESTONE_TAG  78300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b78298_tag(void)
{
	return B78298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_78300 - report soft milestone product tag for wave 78300.
 *
 * Always returns 78300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 78300.
 */
uint32_t
gj_milestone_tag_78300(void)
{
	(void)NULL;
	return b78298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_78300(void)
    __attribute__((alias("gj_milestone_tag_78300")));
