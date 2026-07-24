/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28398: milestone 28400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_28400(void);
 *     - Returns the milestone tag for the milestone 28400 continuum
 *       (always 28400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_28400  (alias)
 *   __libcgj_batch28398_marker = "libcgj-batch28398"
 *
 * Milestone 28400 exclusive continuum CREATE-ONLY (28391-28400). Unique
 * gj_milestone_tag_28400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_28300 / gj_milestone_tag_28200 / gj_milestone_tag_28100,
 * gj_graph_milestone_28400 (batch28400), and gj_continuum_wave_28400
 * (batch28399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28391–28400): host soft probes
 * (cgj_soft_milestone_28400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28398. */
const char __libcgj_batch28398_marker[] = "libcgj-batch28398";

/* Soft milestone tag for wave 28400 (product identity = 28400). */
#define B28398_MILESTONE_TAG  28400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28398_tag(void)
{
	return B28398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_28400 - report soft milestone product tag for wave 28400.
 *
 * Always returns 28400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 28400.
 */
uint32_t
gj_milestone_tag_28400(void)
{
	(void)NULL;
	return b28398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_28400(void)
    __attribute__((alias("gj_milestone_tag_28400")));
