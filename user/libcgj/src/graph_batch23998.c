/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23998: milestone 24000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_24000(void);
 *     - Returns the milestone tag for the milestone 24000 continuum
 *       (always 24000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_24000  (alias)
 *   __libcgj_batch23998_marker = "libcgj-batch23998"
 *
 * Milestone 24000 exclusive continuum CREATE-ONLY (23991-24000). Unique
 * gj_milestone_tag_24000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_23900 / gj_milestone_tag_23800 / gj_milestone_tag_23700,
 * gj_graph_milestone_24000 (batch24000), and gj_continuum_wave_24000
 * (batch23999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (23991–24000): host soft probes
 * (cgj_soft_milestone_24000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=24000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 23998. */
const char __libcgj_batch23998_marker[] = "libcgj-batch23998";

/* Soft milestone tag for wave 24000 (product identity = 24000). */
#define B23998_MILESTONE_TAG  24000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23998_tag(void)
{
	return B23998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_24000 - report soft milestone product tag for wave 24000.
 *
 * Always returns 24000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 24000.
 */
uint32_t
gj_milestone_tag_24000(void)
{
	(void)NULL;
	return b23998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_24000(void)
    __attribute__((alias("gj_milestone_tag_24000")));
