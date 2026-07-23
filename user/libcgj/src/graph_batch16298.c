/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16298: milestone 16300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_16300(void);
 *     - Returns the milestone tag for the milestone 16300 continuum
 *       (always 16300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_16300  (alias)
 *   __libcgj_batch16298_marker = "libcgj-batch16298"
 *
 * Milestone 16300 exclusive continuum CREATE-ONLY (16291-16300). Unique
 * gj_milestone_tag_16300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_16200 / gj_milestone_tag_16100 / gj_milestone_tag_16000,
 * gj_graph_milestone_16300 (batch16300), and gj_continuum_wave_16300
 * (batch16299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (16291–16300): host soft probes
 * (cgj_soft_milestone_16300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=16300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16298. */
const char __libcgj_batch16298_marker[] = "libcgj-batch16298";

/* Soft milestone tag for wave 16300 (product identity = 16300). */
#define B16298_MILESTONE_TAG  16300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16298_tag(void)
{
	return B16298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_16300 - report soft milestone product tag for wave 16300.
 *
 * Always returns 16300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 16300.
 */
uint32_t
gj_milestone_tag_16300(void)
{
	(void)NULL;
	return b16298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_16300(void)
    __attribute__((alias("gj_milestone_tag_16300")));
