/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17298: milestone 17300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_17300(void);
 *     - Returns the milestone tag for the milestone 17300 continuum
 *       (always 17300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_17300  (alias)
 *   __libcgj_batch17298_marker = "libcgj-batch17298"
 *
 * Milestone 17300 exclusive continuum CREATE-ONLY (17291-17300). Unique
 * gj_milestone_tag_17300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_17200 / gj_milestone_tag_17100 / gj_milestone_tag_17000,
 * gj_graph_milestone_17300 (batch17300), and gj_continuum_wave_17300
 * (batch17299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (17291–17300): host soft probes
 * (cgj_soft_milestone_17300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=17300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 17298. */
const char __libcgj_batch17298_marker[] = "libcgj-batch17298";

/* Soft milestone tag for wave 17300 (product identity = 17300). */
#define B17298_MILESTONE_TAG  17300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17298_tag(void)
{
	return B17298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_17300 - report soft milestone product tag for wave 17300.
 *
 * Always returns 17300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 17300.
 */
uint32_t
gj_milestone_tag_17300(void)
{
	(void)NULL;
	return b17298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_17300(void)
    __attribute__((alias("gj_milestone_tag_17300")));
