/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20098: milestone 20100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_20100(void);
 *     - Returns the milestone tag for the milestone 20100 continuum
 *       (always 20100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_20100  (alias)
 *   __libcgj_batch20098_marker = "libcgj-batch20098"
 *
 * Milestone 20100 exclusive continuum CREATE-ONLY (20091-20100). Unique
 * gj_milestone_tag_20100 surface only; no multi-def. Distinct from
 * gj_milestone_tag_20000 / gj_milestone_tag_19900 / gj_milestone_tag_19800,
 * gj_graph_milestone_20100 (batch20100), and gj_continuum_wave_20100
 * (batch20099).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20091–20100): host soft probes
 * (cgj_soft_milestone_20100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20098. */
const char __libcgj_batch20098_marker[] = "libcgj-batch20098";

/* Soft milestone tag for wave 20100 (product identity = 20100). */
#define B20098_MILESTONE_TAG  20100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20098_tag(void)
{
	return B20098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_20100 - report soft milestone product tag for wave 20100.
 *
 * Always returns 20100u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 20100.
 */
uint32_t
gj_milestone_tag_20100(void)
{
	(void)NULL;
	return b20098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_20100(void)
    __attribute__((alias("gj_milestone_tag_20100")));
