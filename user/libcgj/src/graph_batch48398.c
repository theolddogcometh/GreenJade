/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48398: milestone 48400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_48400(void);
 *     - Returns the milestone tag for the milestone 48400 continuum
 *       (always 48400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_48400  (alias)
 *   __libcgj_batch48398_marker = "libcgj-batch48398"
 *
 * Milestone 48400 exclusive continuum CREATE-ONLY (48391-48400). Unique
 * gj_milestone_tag_48400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_48300 / gj_milestone_tag_48200 / gj_milestone_tag_48100,
 * gj_graph_milestone_48400 (batch48400), and gj_continuum_wave_48400
 * (batch48399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (48391–48400): host soft probes
 * (cgj_soft_milestone_48400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=48400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 48398. */
const char __libcgj_batch48398_marker[] = "libcgj-batch48398";

/* Soft milestone tag for wave 48400 (product identity = 48400). */
#define B48398_MILESTONE_TAG  48400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48398_tag(void)
{
	return B48398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_48400 - report soft milestone product tag for wave 48400.
 *
 * Always returns 48400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 48400.
 */
uint32_t
gj_milestone_tag_48400(void)
{
	(void)NULL;
	return b48398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_48400(void)
    __attribute__((alias("gj_milestone_tag_48400")));
