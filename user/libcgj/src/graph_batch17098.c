/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17098: milestone 17100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_17100(void);
 *     - Returns the milestone tag for the milestone 17100 continuum
 *       (always 17100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_17100  (alias)
 *   __libcgj_batch17098_marker = "libcgj-batch17098"
 *
 * Milestone 17100 exclusive continuum CREATE-ONLY (17091-17100). Unique
 * gj_milestone_tag_17100 surface only; no multi-def. Distinct from
 * gj_milestone_tag_17000 / gj_milestone_tag_16900 / gj_milestone_tag_16800,
 * gj_graph_milestone_17100 (batch17100), and gj_continuum_wave_17100
 * (batch17099).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (17091–17100): host soft probes
 * (cgj_soft_milestone_17100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=17100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 17098. */
const char __libcgj_batch17098_marker[] = "libcgj-batch17098";

/* Soft milestone tag for wave 17100 (product identity = 17100). */
#define B17098_MILESTONE_TAG  17100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17098_tag(void)
{
	return B17098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_17100 - report soft milestone product tag for wave 17100.
 *
 * Always returns 17100u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 17100.
 */
uint32_t
gj_milestone_tag_17100(void)
{
	(void)NULL;
	return b17098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_17100(void)
    __attribute__((alias("gj_milestone_tag_17100")));
