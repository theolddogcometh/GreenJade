/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21598: milestone 21600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_21600(void);
 *     - Returns the milestone tag for the milestone 21600 continuum
 *       (always 21600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_21600  (alias)
 *   __libcgj_batch21598_marker = "libcgj-batch21598"
 *
 * Milestone 21600 exclusive continuum CREATE-ONLY (21591-21600). Unique
 * gj_milestone_tag_21600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_21500 / gj_milestone_tag_21400 / gj_milestone_tag_21300,
 * gj_graph_milestone_21600 (batch21600), and gj_continuum_wave_21600
 * (batch21599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21591–21600): host soft probes
 * (cgj_soft_milestone_21600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21598. */
const char __libcgj_batch21598_marker[] = "libcgj-batch21598";

/* Soft milestone tag for wave 21600 (product identity = 21600). */
#define B21598_MILESTONE_TAG  21600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21598_tag(void)
{
	return B21598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_21600 - report soft milestone product tag for wave 21600.
 *
 * Always returns 21600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 21600.
 */
uint32_t
gj_milestone_tag_21600(void)
{
	(void)NULL;
	return b21598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_21600(void)
    __attribute__((alias("gj_milestone_tag_21600")));
