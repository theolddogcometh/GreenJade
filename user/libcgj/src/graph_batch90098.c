/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch90098: milestone 90100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_90100(void);
 *     - Returns the milestone tag for the milestone 90100 continuum
 *       (always 90100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_90100  (alias)
 *   __libcgj_batch90098_marker = "libcgj-batch90098"
 *
 * Milestone 90100 exclusive continuum CREATE-ONLY (25891-90100). Unique
 * gj_milestone_tag_90100 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_90100 (batch90100), and gj_continuum_wave_90100
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–90100): host soft probes
 * (cgj_soft_milestone_90100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=90100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 90098. */
const char __libcgj_batch90098_marker[] = "libcgj-batch90098";

/* Soft milestone tag for wave 90100 (product identity = 90100). */
#define B90098_MILESTONE_TAG  90100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b90098_tag(void)
{
	return B90098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_90100 - report soft milestone product tag for wave 90100.
 *
 * Always returns 90100u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 90100.
 */
uint32_t
gj_milestone_tag_90100(void)
{
	(void)NULL;
	return b90098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_90100(void)
    __attribute__((alias("gj_milestone_tag_90100")));
