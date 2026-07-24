/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21998: milestone 22000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_22000(void);
 *     - Returns the milestone tag for the milestone 22000 continuum
 *       (always 22000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_22000  (alias)
 *   __libcgj_batch21998_marker = "libcgj-batch21998"
 *
 * Milestone 22000 exclusive continuum CREATE-ONLY (21991-22000). Unique
 * gj_milestone_tag_22000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_21900 / gj_milestone_tag_21800 / gj_milestone_tag_21700,
 * gj_graph_milestone_22000 (batch22000), and gj_continuum_wave_22000
 * (batch21999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21991–22000): host soft probes
 * (cgj_soft_milestone_22000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=22000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21998. */
const char __libcgj_batch21998_marker[] = "libcgj-batch21998";

/* Soft milestone tag for wave 22000 (product identity = 22000). */
#define B21998_MILESTONE_TAG  22000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21998_tag(void)
{
	return B21998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_22000 - report soft milestone product tag for wave 22000.
 *
 * Always returns 22000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 22000.
 */
uint32_t
gj_milestone_tag_22000(void)
{
	(void)NULL;
	return b21998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_22000(void)
    __attribute__((alias("gj_milestone_tag_22000")));
