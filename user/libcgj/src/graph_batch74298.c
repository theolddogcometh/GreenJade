/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch74298: milestone 74300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_74300(void);
 *     - Returns the milestone tag for the milestone 74300 continuum
 *       (always 74300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_74300  (alias)
 *   __libcgj_batch74298_marker = "libcgj-batch74298"
 *
 * Milestone 74300 exclusive continuum CREATE-ONLY (25891-74300). Unique
 * gj_milestone_tag_74300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_74300 (batch74300), and gj_continuum_wave_74300
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–74300): host soft probes
 * (cgj_soft_milestone_74300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=74300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 74298. */
const char __libcgj_batch74298_marker[] = "libcgj-batch74298";

/* Soft milestone tag for wave 74300 (product identity = 74300). */
#define B74298_MILESTONE_TAG  74300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b74298_tag(void)
{
	return B74298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_74300 - report soft milestone product tag for wave 74300.
 *
 * Always returns 74300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 74300.
 */
uint32_t
gj_milestone_tag_74300(void)
{
	(void)NULL;
	return b74298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_74300(void)
    __attribute__((alias("gj_milestone_tag_74300")));
