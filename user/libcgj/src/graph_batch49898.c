/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49898: milestone 49900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_49900(void);
 *     - Returns the milestone tag for the milestone 49900 continuum
 *       (always 49900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_49900  (alias)
 *   __libcgj_batch49898_marker = "libcgj-batch49898"
 *
 * Milestone 49900 exclusive continuum CREATE-ONLY (49891-49900). Unique
 * gj_milestone_tag_49900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_49800 / gj_milestone_tag_49700 / gj_milestone_tag_49600,
 * gj_graph_milestone_49900 (batch49900), and gj_continuum_wave_49900
 * (batch49899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (49891–49900): host soft probes
 * (cgj_soft_milestone_49900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=49900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 49898. */
const char __libcgj_batch49898_marker[] = "libcgj-batch49898";

/* Soft milestone tag for wave 49900 (product identity = 49900). */
#define B49898_MILESTONE_TAG  49900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49898_tag(void)
{
	return B49898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_49900 - report soft milestone product tag for wave 49900.
 *
 * Always returns 49900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 49900.
 */
uint32_t
gj_milestone_tag_49900(void)
{
	(void)NULL;
	return b49898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_49900(void)
    __attribute__((alias("gj_milestone_tag_49900")));
