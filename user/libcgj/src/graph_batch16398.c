/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16398: milestone 16400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_16400(void);
 *     - Returns the milestone tag for the milestone 16400 continuum
 *       (always 16400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_16400  (alias)
 *   __libcgj_batch16398_marker = "libcgj-batch16398"
 *
 * Milestone 16400 exclusive continuum CREATE-ONLY (16391-16400). Unique
 * gj_milestone_tag_16400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_16300 / gj_milestone_tag_16200 / gj_milestone_tag_16100,
 * gj_graph_milestone_16400 (batch16400), and gj_continuum_wave_16400
 * (batch16399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (16391–16400): host soft probes
 * (cgj_soft_milestone_16400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=16400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16398. */
const char __libcgj_batch16398_marker[] = "libcgj-batch16398";

/* Soft milestone tag for wave 16400 (product identity = 16400). */
#define B16398_MILESTONE_TAG  16400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16398_tag(void)
{
	return B16398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_16400 - report soft milestone product tag for wave 16400.
 *
 * Always returns 16400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 16400.
 */
uint32_t
gj_milestone_tag_16400(void)
{
	(void)NULL;
	return b16398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_16400(void)
    __attribute__((alias("gj_milestone_tag_16400")));
