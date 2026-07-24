/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42298: milestone 42300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_42300(void);
 *     - Returns the milestone tag for the milestone 42300 continuum
 *       (always 42300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_42300  (alias)
 *   __libcgj_batch42298_marker = "libcgj-batch42298"
 *
 * Milestone 42300 exclusive continuum CREATE-ONLY (42291-42300). Unique
 * gj_milestone_tag_42300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_42200 / gj_milestone_tag_42100 / gj_milestone_tag_42000,
 * gj_graph_milestone_42300 (batch42300), and gj_continuum_wave_42300
 * (batch42299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (42291–42300): host soft probes
 * (cgj_soft_milestone_42300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=42300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 42298. */
const char __libcgj_batch42298_marker[] = "libcgj-batch42298";

/* Soft milestone tag for wave 42300 (product identity = 42300). */
#define B42298_MILESTONE_TAG  42300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42298_tag(void)
{
	return B42298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_42300 - report soft milestone product tag for wave 42300.
 *
 * Always returns 42300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 42300.
 */
uint32_t
gj_milestone_tag_42300(void)
{
	(void)NULL;
	return b42298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_42300(void)
    __attribute__((alias("gj_milestone_tag_42300")));
