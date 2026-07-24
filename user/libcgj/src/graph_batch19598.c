/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19598: milestone 19600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_19600(void);
 *     - Returns the milestone tag for the milestone 19600 continuum
 *       (always 19600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_19600  (alias)
 *   __libcgj_batch19598_marker = "libcgj-batch19598"
 *
 * Milestone 19600 exclusive continuum CREATE-ONLY (19591-19600). Unique
 * gj_milestone_tag_19600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_19500 / gj_milestone_tag_19400 / gj_milestone_tag_19300,
 * gj_graph_milestone_19600 (batch19600), and gj_continuum_wave_19600
 * (batch19599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (19591–19600): host soft probes
 * (cgj_soft_milestone_19600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=19600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 19598. */
const char __libcgj_batch19598_marker[] = "libcgj-batch19598";

/* Soft milestone tag for wave 19600 (product identity = 19600). */
#define B19598_MILESTONE_TAG  19600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19598_tag(void)
{
	return B19598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_19600 - report soft milestone product tag for wave 19600.
 *
 * Always returns 19600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 19600.
 */
uint32_t
gj_milestone_tag_19600(void)
{
	(void)NULL;
	return b19598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_19600(void)
    __attribute__((alias("gj_milestone_tag_19600")));
