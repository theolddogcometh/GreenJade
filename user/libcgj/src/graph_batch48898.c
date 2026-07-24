/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48898: milestone 48900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_48900(void);
 *     - Returns the milestone tag for the milestone 48900 continuum
 *       (always 48900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_48900  (alias)
 *   __libcgj_batch48898_marker = "libcgj-batch48898"
 *
 * Milestone 48900 exclusive continuum CREATE-ONLY (48891-48900). Unique
 * gj_milestone_tag_48900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_48800 / gj_milestone_tag_48700 / gj_milestone_tag_48600,
 * gj_graph_milestone_48900 (batch48900), and gj_continuum_wave_48900
 * (batch48899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (48891–48900): host soft probes
 * (cgj_soft_milestone_48900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=48900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 48898. */
const char __libcgj_batch48898_marker[] = "libcgj-batch48898";

/* Soft milestone tag for wave 48900 (product identity = 48900). */
#define B48898_MILESTONE_TAG  48900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48898_tag(void)
{
	return B48898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_48900 - report soft milestone product tag for wave 48900.
 *
 * Always returns 48900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 48900.
 */
uint32_t
gj_milestone_tag_48900(void)
{
	(void)NULL;
	return b48898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_48900(void)
    __attribute__((alias("gj_milestone_tag_48900")));
