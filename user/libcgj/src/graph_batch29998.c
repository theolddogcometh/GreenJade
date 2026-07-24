/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29998: milestone 30000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_30000(void);
 *     - Returns the milestone tag for the milestone 30000 continuum
 *       (always 30000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_30000  (alias)
 *   __libcgj_batch29998_marker = "libcgj-batch29998"
 *
 * Milestone 30000 exclusive continuum CREATE-ONLY (29991-30000). Unique
 * gj_milestone_tag_30000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_29900 / gj_milestone_tag_29800 / gj_milestone_tag_29700,
 * gj_graph_milestone_30000 (batch30000), and gj_continuum_wave_30000
 * (batch29999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (29991–30000): host soft probes
 * (cgj_soft_milestone_30000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 29998. */
const char __libcgj_batch29998_marker[] = "libcgj-batch29998";

/* Soft milestone tag for wave 30000 (product identity = 30000). */
#define B29998_MILESTONE_TAG  30000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29998_tag(void)
{
	return B29998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_30000 - report soft milestone product tag for wave 30000.
 *
 * Always returns 30000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 30000.
 */
uint32_t
gj_milestone_tag_30000(void)
{
	(void)NULL;
	return b29998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_30000(void)
    __attribute__((alias("gj_milestone_tag_30000")));
