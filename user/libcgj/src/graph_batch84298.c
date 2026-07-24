/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch84298: milestone 84300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_84300(void);
 *     - Returns the milestone tag for the milestone 84300 continuum
 *       (always 84300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_84300  (alias)
 *   __libcgj_batch84298_marker = "libcgj-batch84298"
 *
 * Milestone 84300 exclusive continuum CREATE-ONLY (25891-84300). Unique
 * gj_milestone_tag_84300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_84300 (batch84300), and gj_continuum_wave_84300
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–84300): host soft probes
 * (cgj_soft_milestone_84300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=84300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 84298. */
const char __libcgj_batch84298_marker[] = "libcgj-batch84298";

/* Soft milestone tag for wave 84300 (product identity = 84300). */
#define B84298_MILESTONE_TAG  84300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b84298_tag(void)
{
	return B84298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_84300 - report soft milestone product tag for wave 84300.
 *
 * Always returns 84300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 84300.
 */
uint32_t
gj_milestone_tag_84300(void)
{
	(void)NULL;
	return b84298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_84300(void)
    __attribute__((alias("gj_milestone_tag_84300")));
