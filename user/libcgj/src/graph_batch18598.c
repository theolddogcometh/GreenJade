/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18598: milestone 18600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_18600(void);
 *     - Returns the milestone tag for the milestone 18600 continuum
 *       (always 18600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_18600  (alias)
 *   __libcgj_batch18598_marker = "libcgj-batch18598"
 *
 * Milestone 18600 exclusive continuum CREATE-ONLY (18591-18600). Unique
 * gj_milestone_tag_18600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_18400 / gj_milestone_tag_18300 / gj_milestone_tag_18200,
 * gj_graph_milestone_18600 (batch18600), and gj_continuum_wave_18600
 * (batch18599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18591–18600): host soft probes
 * (cgj_soft_milestone_18600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=18600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18598. */
const char __libcgj_batch18598_marker[] = "libcgj-batch18598";

/* Soft milestone tag for wave 18600 (product identity = 18600). */
#define B18598_MILESTONE_TAG  18600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18598_tag(void)
{
	return B18598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_18600 - report soft milestone product tag for wave 18600.
 *
 * Always returns 18600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 18600.
 */
uint32_t
gj_milestone_tag_18600(void)
{
	(void)NULL;
	return b18598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_18600(void)
    __attribute__((alias("gj_milestone_tag_18600")));
