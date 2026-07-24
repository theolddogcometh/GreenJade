/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch88298: milestone 88300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_88300(void);
 *     - Returns the milestone tag for the milestone 88300 continuum
 *       (always 88300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_88300  (alias)
 *   __libcgj_batch88298_marker = "libcgj-batch88298"
 *
 * Milestone 88300 exclusive continuum CREATE-ONLY (25891-88300). Unique
 * gj_milestone_tag_88300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_88300 (batch88300), and gj_continuum_wave_88300
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–88300): host soft probes
 * (cgj_soft_milestone_88300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=88300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 88298. */
const char __libcgj_batch88298_marker[] = "libcgj-batch88298";

/* Soft milestone tag for wave 88300 (product identity = 88300). */
#define B88298_MILESTONE_TAG  88300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b88298_tag(void)
{
	return B88298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_88300 - report soft milestone product tag for wave 88300.
 *
 * Always returns 88300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 88300.
 */
uint32_t
gj_milestone_tag_88300(void)
{
	(void)NULL;
	return b88298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_88300(void)
    __attribute__((alias("gj_milestone_tag_88300")));
