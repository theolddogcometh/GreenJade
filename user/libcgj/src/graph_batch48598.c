/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48598: milestone 48600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_48600(void);
 *     - Returns the milestone tag for the milestone 48600 continuum
 *       (always 48600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_48600  (alias)
 *   __libcgj_batch48598_marker = "libcgj-batch48598"
 *
 * Milestone 48600 exclusive continuum CREATE-ONLY (48591-48600). Unique
 * gj_milestone_tag_48600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_48500 / gj_milestone_tag_48400 / gj_milestone_tag_48300,
 * gj_graph_milestone_48600 (batch48600), and gj_continuum_wave_48600
 * (batch48599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (48591–48600): host soft probes
 * (cgj_soft_milestone_48600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=48600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 48598. */
const char __libcgj_batch48598_marker[] = "libcgj-batch48598";

/* Soft milestone tag for wave 48600 (product identity = 48600). */
#define B48598_MILESTONE_TAG  48600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48598_tag(void)
{
	return B48598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_48600 - report soft milestone product tag for wave 48600.
 *
 * Always returns 48600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 48600.
 */
uint32_t
gj_milestone_tag_48600(void)
{
	(void)NULL;
	return b48598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_48600(void)
    __attribute__((alias("gj_milestone_tag_48600")));
