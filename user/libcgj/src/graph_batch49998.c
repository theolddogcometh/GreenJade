/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49998: milestone 50000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_50000(void);
 *     - Returns the milestone tag for the milestone 50000 continuum
 *       (always 50000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_50000  (alias)
 *   __libcgj_batch49998_marker = "libcgj-batch49998"
 *
 * Milestone 50000 exclusive continuum CREATE-ONLY (49991-50000). Unique
 * gj_milestone_tag_50000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_49900 / gj_milestone_tag_49800 / gj_milestone_tag_49700,
 * gj_graph_milestone_50000 (batch50000), and gj_continuum_wave_50000
 * (batch49999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (49991–50000): host soft probes
 * (cgj_soft_milestone_50000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=50000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 49998. */
const char __libcgj_batch49998_marker[] = "libcgj-batch49998";

/* Soft milestone tag for wave 50000 (product identity = 50000). */
#define B49998_MILESTONE_TAG  50000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49998_tag(void)
{
	return B49998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_50000 - report soft milestone product tag for wave 50000.
 *
 * Always returns 50000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 50000.
 */
uint32_t
gj_milestone_tag_50000(void)
{
	(void)NULL;
	return b49998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_50000(void)
    __attribute__((alias("gj_milestone_tag_50000")));
