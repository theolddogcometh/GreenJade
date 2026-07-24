/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18398: milestone 18400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_18400(void);
 *     - Returns the milestone tag for the milestone 18400 continuum
 *       (always 18400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_18400  (alias)
 *   __libcgj_batch18398_marker = "libcgj-batch18398"
 *
 * Milestone 18400 exclusive continuum CREATE-ONLY (18391-18400). Unique
 * gj_milestone_tag_18400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_18200 / gj_milestone_tag_18100 / gj_milestone_tag_18000,
 * gj_graph_milestone_18400 (batch18400), and gj_continuum_wave_18400
 * (batch18399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18391–18400): host soft probes
 * (cgj_soft_milestone_18400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=18400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18398. */
const char __libcgj_batch18398_marker[] = "libcgj-batch18398";

/* Soft milestone tag for wave 18400 (product identity = 18400). */
#define B18398_MILESTONE_TAG  18400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18398_tag(void)
{
	return B18398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_18400 - report soft milestone product tag for wave 18400.
 *
 * Always returns 18400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 18400.
 */
uint32_t
gj_milestone_tag_18400(void)
{
	(void)NULL;
	return b18398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_18400(void)
    __attribute__((alias("gj_milestone_tag_18400")));
