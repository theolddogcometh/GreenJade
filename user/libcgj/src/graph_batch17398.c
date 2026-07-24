/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17398: milestone 17400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_17400(void);
 *     - Returns the milestone tag for the milestone 17400 continuum
 *       (always 17400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_17400  (alias)
 *   __libcgj_batch17398_marker = "libcgj-batch17398"
 *
 * Milestone 17400 exclusive continuum CREATE-ONLY (17391-17400). Unique
 * gj_milestone_tag_17400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_17300 / gj_milestone_tag_17200 / gj_milestone_tag_17100,
 * gj_graph_milestone_17400 (batch17400), and gj_continuum_wave_17400
 * (batch17399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (17391–17400): host soft probes
 * (cgj_soft_milestone_17400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=17400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 17398. */
const char __libcgj_batch17398_marker[] = "libcgj-batch17398";

/* Soft milestone tag for wave 17400 (product identity = 17400). */
#define B17398_MILESTONE_TAG  17400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17398_tag(void)
{
	return B17398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_17400 - report soft milestone product tag for wave 17400.
 *
 * Always returns 17400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 17400.
 */
uint32_t
gj_milestone_tag_17400(void)
{
	(void)NULL;
	return b17398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_17400(void)
    __attribute__((alias("gj_milestone_tag_17400")));
