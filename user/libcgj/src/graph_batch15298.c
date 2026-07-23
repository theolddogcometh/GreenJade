/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15298: milestone 15300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_15300(void);
 *     - Returns the milestone tag for the milestone 15300 continuum
 *       (always 15300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_15300  (alias)
 *   __libcgj_batch15298_marker = "libcgj-batch15298"
 *
 * Milestone 15300 exclusive continuum CREATE-ONLY (15291-15300). Unique
 * gj_milestone_tag_15300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_15200 / gj_milestone_tag_15100 / gj_milestone_tag_15000,
 * gj_graph_milestone_15300 (batch15300), and gj_continuum_wave_15300
 * (batch15299). No parent wires. No __int128.
 *
 * CGJ soft marker band (15291–15300): host soft probes
 * (cgj_soft_milestone_15300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15298. */
const char __libcgj_batch15298_marker[] = "libcgj-batch15298";

/* Soft milestone tag for wave 15300 (product identity = 15300). */
#define B15298_MILESTONE_TAG  15300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15298_tag(void)
{
	return B15298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_15300 - report soft milestone product tag for wave 15300.
 *
 * Always returns 15300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 15300.
 */
uint32_t
gj_milestone_tag_15300(void)
{
	(void)NULL;
	return b15298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_15300(void)
    __attribute__((alias("gj_milestone_tag_15300")));
