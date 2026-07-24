/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23298: milestone 23300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_23300(void);
 *     - Returns the milestone tag for the milestone 23300 continuum
 *       (always 23300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_23300  (alias)
 *   __libcgj_batch23298_marker = "libcgj-batch23298"
 *
 * Milestone 23300 exclusive continuum CREATE-ONLY (23291-23300). Unique
 * gj_milestone_tag_23300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_23200 / gj_milestone_tag_23100 / gj_milestone_tag_23000,
 * gj_graph_milestone_23300 (batch23300), and gj_continuum_wave_23300
 * (batch23299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (23291–23300): host soft probes
 * (cgj_soft_milestone_23300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=23300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 23298. */
const char __libcgj_batch23298_marker[] = "libcgj-batch23298";

/* Soft milestone tag for wave 23300 (product identity = 23300). */
#define B23298_MILESTONE_TAG  23300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23298_tag(void)
{
	return B23298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_23300 - report soft milestone product tag for wave 23300.
 *
 * Always returns 23300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 23300.
 */
uint32_t
gj_milestone_tag_23300(void)
{
	(void)NULL;
	return b23298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_23300(void)
    __attribute__((alias("gj_milestone_tag_23300")));
