/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28898: milestone 28900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_28900(void);
 *     - Returns the milestone tag for the milestone 28900 continuum
 *       (always 28900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_28900  (alias)
 *   __libcgj_batch28898_marker = "libcgj-batch28898"
 *
 * Milestone 28900 exclusive continuum CREATE-ONLY (28891-28900). Unique
 * gj_milestone_tag_28900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_28800 / gj_milestone_tag_28700 / gj_milestone_tag_28600,
 * gj_graph_milestone_28900 (batch28900), and gj_continuum_wave_28900
 * (batch28899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28891–28900): host soft probes
 * (cgj_soft_milestone_28900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28898. */
const char __libcgj_batch28898_marker[] = "libcgj-batch28898";

/* Soft milestone tag for wave 28900 (product identity = 28900). */
#define B28898_MILESTONE_TAG  28900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28898_tag(void)
{
	return B28898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_28900 - report soft milestone product tag for wave 28900.
 *
 * Always returns 28900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 28900.
 */
uint32_t
gj_milestone_tag_28900(void)
{
	(void)NULL;
	return b28898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_28900(void)
    __attribute__((alias("gj_milestone_tag_28900")));
