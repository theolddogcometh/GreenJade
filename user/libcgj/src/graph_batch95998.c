/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95998: milestone 96000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_96000(void);
 *     - Returns the milestone tag for the milestone 96000 continuum
 *       (always 96000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_96000  (alias)
 *   __libcgj_batch95998_marker = "libcgj-batch95998"
 *
 * Milestone 96000 exclusive continuum CREATE-ONLY (25891-96000). Unique
 * gj_milestone_tag_96000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_96000 (batch96000), and gj_continuum_wave_96000
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–96000): host soft probes
 * (cgj_soft_milestone_96000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=96000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 95998. */
const char __libcgj_batch95998_marker[] = "libcgj-batch95998";

/* Soft milestone tag for wave 96000 (product identity = 96000). */
#define B95998_MILESTONE_TAG  96000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b95998_tag(void)
{
	return B95998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_96000 - report soft milestone product tag for wave 96000.
 *
 * Always returns 96000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 96000.
 */
uint32_t
gj_milestone_tag_96000(void)
{
	(void)NULL;
	return b95998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_96000(void)
    __attribute__((alias("gj_milestone_tag_96000")));
