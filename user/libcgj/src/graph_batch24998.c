/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24998: milestone 25000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_25000(void);
 *     - Returns the milestone tag for the milestone 25000 continuum
 *       (always 25000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_25000  (alias)
 *   __libcgj_batch24998_marker = "libcgj-batch24998"
 *
 * Milestone 25000 exclusive continuum CREATE-ONLY (24991-25000). Unique
 * gj_milestone_tag_25000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_24900 / gj_milestone_tag_24800 / gj_milestone_tag_24700,
 * gj_graph_milestone_25000 (batch25000), and gj_continuum_wave_25000
 * (batch24999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (24991–25000): host soft probes
 * (cgj_soft_milestone_25000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24998. */
const char __libcgj_batch24998_marker[] = "libcgj-batch24998";

/* Soft milestone tag for wave 25000 (product identity = 25000). */
#define B24998_MILESTONE_TAG  25000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24998_tag(void)
{
	return B24998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_25000 - report soft milestone product tag for wave 25000.
 *
 * Always returns 25000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 25000.
 */
uint32_t
gj_milestone_tag_25000(void)
{
	(void)NULL;
	return b24998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_25000(void)
    __attribute__((alias("gj_milestone_tag_25000")));
