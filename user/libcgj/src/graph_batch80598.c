/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch80598: milestone 80600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_80600(void);
 *     - Returns the milestone tag for the milestone 80600 continuum
 *       (always 80600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_80600  (alias)
 *   __libcgj_batch80598_marker = "libcgj-batch80598"
 *
 * Milestone 80600 exclusive continuum CREATE-ONLY (25891-80600). Unique
 * gj_milestone_tag_80600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_80600 (batch80600), and gj_continuum_wave_80600
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–80600): host soft probes
 * (cgj_soft_milestone_80600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=80600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 80598. */
const char __libcgj_batch80598_marker[] = "libcgj-batch80598";

/* Soft milestone tag for wave 80600 (product identity = 80600). */
#define B80598_MILESTONE_TAG  80600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b80598_tag(void)
{
	return B80598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_80600 - report soft milestone product tag for wave 80600.
 *
 * Always returns 80600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 80600.
 */
uint32_t
gj_milestone_tag_80600(void)
{
	(void)NULL;
	return b80598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_80600(void)
    __attribute__((alias("gj_milestone_tag_80600")));
