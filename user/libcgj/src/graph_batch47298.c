/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch47298: milestone 47300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_47300(void);
 *     - Returns the milestone tag for the milestone 47300 continuum
 *       (always 47300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_47300  (alias)
 *   __libcgj_batch47298_marker = "libcgj-batch47298"
 *
 * Milestone 47300 exclusive continuum CREATE-ONLY (47291-47300). Unique
 * gj_milestone_tag_47300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_47200 / gj_milestone_tag_47100 / gj_milestone_tag_47000,
 * gj_graph_milestone_47300 (batch47300), and gj_continuum_wave_47300
 * (batch47299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (47291–47300): host soft probes
 * (cgj_soft_milestone_47300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=47300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 47298. */
const char __libcgj_batch47298_marker[] = "libcgj-batch47298";

/* Soft milestone tag for wave 47300 (product identity = 47300). */
#define B47298_MILESTONE_TAG  47300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b47298_tag(void)
{
	return B47298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_47300 - report soft milestone product tag for wave 47300.
 *
 * Always returns 47300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 47300.
 */
uint32_t
gj_milestone_tag_47300(void)
{
	(void)NULL;
	return b47298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_47300(void)
    __attribute__((alias("gj_milestone_tag_47300")));
