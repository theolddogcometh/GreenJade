/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42398: milestone 42400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_42400(void);
 *     - Returns the milestone tag for the milestone 42400 continuum
 *       (always 42400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_42400  (alias)
 *   __libcgj_batch42398_marker = "libcgj-batch42398"
 *
 * Milestone 42400 exclusive continuum CREATE-ONLY (42391-42400). Unique
 * gj_milestone_tag_42400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_42300 / gj_milestone_tag_42200 / gj_milestone_tag_42100,
 * gj_graph_milestone_42400 (batch42400), and gj_continuum_wave_42400
 * (batch42399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (42391–42400): host soft probes
 * (cgj_soft_milestone_42400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=42400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 42398. */
const char __libcgj_batch42398_marker[] = "libcgj-batch42398";

/* Soft milestone tag for wave 42400 (product identity = 42400). */
#define B42398_MILESTONE_TAG  42400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42398_tag(void)
{
	return B42398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_42400 - report soft milestone product tag for wave 42400.
 *
 * Always returns 42400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 42400.
 */
uint32_t
gj_milestone_tag_42400(void)
{
	(void)NULL;
	return b42398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_42400(void)
    __attribute__((alias("gj_milestone_tag_42400")));
