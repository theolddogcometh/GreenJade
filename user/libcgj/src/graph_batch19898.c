/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19898: milestone 19900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_19900(void);
 *     - Returns the milestone tag for the milestone 19900 continuum
 *       (always 19900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_19900  (alias)
 *   __libcgj_batch19898_marker = "libcgj-batch19898"
 *
 * Milestone 19900 exclusive continuum CREATE-ONLY (19891-19900). Unique
 * gj_milestone_tag_19900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_19800 / gj_milestone_tag_19700 / gj_milestone_tag_19600,
 * gj_graph_milestone_19900 (batch19900), and gj_continuum_wave_19900
 * (batch19899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (19891–19900): host soft probes
 * (cgj_soft_milestone_19900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=19900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 19898. */
const char __libcgj_batch19898_marker[] = "libcgj-batch19898";

/* Soft milestone tag for wave 19900 (product identity = 19900). */
#define B19898_MILESTONE_TAG  19900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19898_tag(void)
{
	return B19898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_19900 - report soft milestone product tag for wave 19900.
 *
 * Always returns 19900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 19900.
 */
uint32_t
gj_milestone_tag_19900(void)
{
	(void)NULL;
	return b19898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_19900(void)
    __attribute__((alias("gj_milestone_tag_19900")));
