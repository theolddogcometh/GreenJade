/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26598: milestone 26600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_26600(void);
 *     - Returns the milestone tag for the milestone 26600 continuum
 *       (always 26600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_26600  (alias)
 *   __libcgj_batch26598_marker = "libcgj-batch26598"
 *
 * Milestone 26600 exclusive continuum CREATE-ONLY (26591-26600). Unique
 * gj_milestone_tag_26600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_26500 / gj_milestone_tag_26400 / gj_milestone_tag_26300,
 * gj_graph_milestone_26600 (batch26600), and gj_continuum_wave_26600
 * (batch26599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26591–26600): host soft probes
 * (cgj_soft_milestone_26600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26598. */
const char __libcgj_batch26598_marker[] = "libcgj-batch26598";

/* Soft milestone tag for wave 26600 (product identity = 26600). */
#define B26598_MILESTONE_TAG  26600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26598_tag(void)
{
	return B26598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_26600 - report soft milestone product tag for wave 26600.
 *
 * Always returns 26600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 26600.
 */
uint32_t
gj_milestone_tag_26600(void)
{
	(void)NULL;
	return b26598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_26600(void)
    __attribute__((alias("gj_milestone_tag_26600")));
