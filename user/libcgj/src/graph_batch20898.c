/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20898: milestone 20900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_20900(void);
 *     - Returns the milestone tag for the milestone 20900 continuum
 *       (always 20900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_20900  (alias)
 *   __libcgj_batch20898_marker = "libcgj-batch20898"
 *
 * Milestone 20900 exclusive continuum CREATE-ONLY (20891-20900). Unique
 * gj_milestone_tag_20900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_20800 / gj_milestone_tag_20700 / gj_milestone_tag_20600,
 * gj_graph_milestone_20900 (batch20900), and gj_continuum_wave_20900
 * (batch20899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20891–20900): host soft probes
 * (cgj_soft_milestone_20900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20898. */
const char __libcgj_batch20898_marker[] = "libcgj-batch20898";

/* Soft milestone tag for wave 20900 (product identity = 20900). */
#define B20898_MILESTONE_TAG  20900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20898_tag(void)
{
	return B20898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_20900 - report soft milestone product tag for wave 20900.
 *
 * Always returns 20900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 20900.
 */
uint32_t
gj_milestone_tag_20900(void)
{
	(void)NULL;
	return b20898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_20900(void)
    __attribute__((alias("gj_milestone_tag_20900")));
