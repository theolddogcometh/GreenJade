/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29898: milestone 29900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_29900(void);
 *     - Returns the milestone tag for the milestone 29900 continuum
 *       (always 29900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_29900  (alias)
 *   __libcgj_batch29898_marker = "libcgj-batch29898"
 *
 * Milestone 29900 exclusive continuum CREATE-ONLY (29891-29900). Unique
 * gj_milestone_tag_29900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_29800 / gj_milestone_tag_29700 / gj_milestone_tag_29600,
 * gj_graph_milestone_29900 (batch29900), and gj_continuum_wave_29900
 * (batch29899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (29891–29900): host soft probes
 * (cgj_soft_milestone_29900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=29900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 29898. */
const char __libcgj_batch29898_marker[] = "libcgj-batch29898";

/* Soft milestone tag for wave 29900 (product identity = 29900). */
#define B29898_MILESTONE_TAG  29900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29898_tag(void)
{
	return B29898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_29900 - report soft milestone product tag for wave 29900.
 *
 * Always returns 29900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 29900.
 */
uint32_t
gj_milestone_tag_29900(void)
{
	(void)NULL;
	return b29898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_29900(void)
    __attribute__((alias("gj_milestone_tag_29900")));
