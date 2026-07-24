/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46998: milestone 47000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_47000(void);
 *     - Returns the milestone tag for the milestone 47000 continuum
 *       (always 47000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_47000  (alias)
 *   __libcgj_batch46998_marker = "libcgj-batch46998"
 *
 * Milestone 47000 exclusive continuum CREATE-ONLY (46991-47000). Unique
 * gj_milestone_tag_47000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_46900 / gj_milestone_tag_46800 / gj_milestone_tag_46700,
 * gj_graph_milestone_47000 (batch47000), and gj_continuum_wave_47000
 * (batch46999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (46991–47000): host soft probes
 * (cgj_soft_milestone_47000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=47000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 46998. */
const char __libcgj_batch46998_marker[] = "libcgj-batch46998";

/* Soft milestone tag for wave 47000 (product identity = 47000). */
#define B46998_MILESTONE_TAG  47000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46998_tag(void)
{
	return B46998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_47000 - report soft milestone product tag for wave 47000.
 *
 * Always returns 47000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 47000.
 */
uint32_t
gj_milestone_tag_47000(void)
{
	(void)NULL;
	return b46998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_47000(void)
    __attribute__((alias("gj_milestone_tag_47000")));
