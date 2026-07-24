/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch59298: milestone 59300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_59300(void);
 *     - Returns the milestone tag for the milestone 59300 continuum
 *       (always 59300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_59300  (alias)
 *   __libcgj_batch59298_marker = "libcgj-batch59298"
 *
 * Milestone 59300 exclusive continuum CREATE-ONLY (25891-59300). Unique
 * gj_milestone_tag_59300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_59300 (batch59300), and gj_continuum_wave_59300
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–59300): host soft probes
 * (cgj_soft_milestone_59300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=59300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 59298. */
const char __libcgj_batch59298_marker[] = "libcgj-batch59298";

/* Soft milestone tag for wave 59300 (product identity = 59300). */
#define B59298_MILESTONE_TAG  59300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b59298_tag(void)
{
	return B59298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_59300 - report soft milestone product tag for wave 59300.
 *
 * Always returns 59300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 59300.
 */
uint32_t
gj_milestone_tag_59300(void)
{
	(void)NULL;
	return b59298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_59300(void)
    __attribute__((alias("gj_milestone_tag_59300")));
