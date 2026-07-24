/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40998: milestone 41000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_41000(void);
 *     - Returns the milestone tag for the milestone 41000 continuum
 *       (always 41000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_41000  (alias)
 *   __libcgj_batch40998_marker = "libcgj-batch40998"
 *
 * Milestone 41000 exclusive continuum CREATE-ONLY (40991-41000). Unique
 * gj_milestone_tag_41000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_40900 / gj_milestone_tag_40800 / gj_milestone_tag_40700,
 * gj_graph_milestone_41000 (batch41000), and gj_continuum_wave_41000
 * (batch40999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40991–41000): host soft probes
 * (cgj_soft_milestone_41000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=41000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40998. */
const char __libcgj_batch40998_marker[] = "libcgj-batch40998";

/* Soft milestone tag for wave 41000 (product identity = 41000). */
#define B40998_MILESTONE_TAG  41000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40998_tag(void)
{
	return B40998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_41000 - report soft milestone product tag for wave 41000.
 *
 * Always returns 41000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 41000.
 */
uint32_t
gj_milestone_tag_41000(void)
{
	(void)NULL;
	return b40998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_41000(void)
    __attribute__((alias("gj_milestone_tag_41000")));
