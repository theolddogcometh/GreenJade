/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25298: milestone 25300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_25300(void);
 *     - Returns the milestone tag for the milestone 25300 continuum
 *       (always 25300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_25300  (alias)
 *   __libcgj_batch25298_marker = "libcgj-batch25298"
 *
 * Milestone 25300 exclusive continuum CREATE-ONLY (25291-25300). Unique
 * gj_milestone_tag_25300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25200 / gj_milestone_tag_25100 / gj_milestone_tag_25000,
 * gj_graph_milestone_25300 (batch25300), and gj_continuum_wave_25300
 * (batch25299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25291–25300): host soft probes
 * (cgj_soft_milestone_25300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25298. */
const char __libcgj_batch25298_marker[] = "libcgj-batch25298";

/* Soft milestone tag for wave 25300 (product identity = 25300). */
#define B25298_MILESTONE_TAG  25300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25298_tag(void)
{
	return B25298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_25300 - report soft milestone product tag for wave 25300.
 *
 * Always returns 25300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 25300.
 */
uint32_t
gj_milestone_tag_25300(void)
{
	(void)NULL;
	return b25298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_25300(void)
    __attribute__((alias("gj_milestone_tag_25300")));
