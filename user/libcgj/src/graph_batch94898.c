/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch94898: milestone 94900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_94900(void);
 *     - Returns the milestone tag for the milestone 94900 continuum
 *       (always 94900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_94900  (alias)
 *   __libcgj_batch94898_marker = "libcgj-batch94898"
 *
 * Milestone 94900 exclusive continuum CREATE-ONLY (25891-94900). Unique
 * gj_milestone_tag_94900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_94900 (batch94900), and gj_continuum_wave_94900
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–94900): host soft probes
 * (cgj_soft_milestone_94900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=94900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 94898. */
const char __libcgj_batch94898_marker[] = "libcgj-batch94898";

/* Soft milestone tag for wave 94900 (product identity = 94900). */
#define B94898_MILESTONE_TAG  94900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b94898_tag(void)
{
	return B94898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_94900 - report soft milestone product tag for wave 94900.
 *
 * Always returns 94900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 94900.
 */
uint32_t
gj_milestone_tag_94900(void)
{
	(void)NULL;
	return b94898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_94900(void)
    __attribute__((alias("gj_milestone_tag_94900")));
