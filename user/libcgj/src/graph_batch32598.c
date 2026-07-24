/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch32598: milestone 32600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_32600(void);
 *     - Returns the milestone tag for the milestone 32600 continuum
 *       (always 32600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_32600  (alias)
 *   __libcgj_batch32598_marker = "libcgj-batch32598"
 *
 * Milestone 32600 exclusive continuum CREATE-ONLY (32591-32600). Unique
 * gj_milestone_tag_32600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_32500 / gj_milestone_tag_32400 / gj_milestone_tag_32300,
 * gj_graph_milestone_32600 (batch32600), and gj_continuum_wave_32600
 * (batch32599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (32591–32600): host soft probes
 * (cgj_soft_milestone_32600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=32600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 32598. */
const char __libcgj_batch32598_marker[] = "libcgj-batch32598";

/* Soft milestone tag for wave 32600 (product identity = 32600). */
#define B32598_MILESTONE_TAG  32600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b32598_tag(void)
{
	return B32598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_32600 - report soft milestone product tag for wave 32600.
 *
 * Always returns 32600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 32600.
 */
uint32_t
gj_milestone_tag_32600(void)
{
	(void)NULL;
	return b32598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_32600(void)
    __attribute__((alias("gj_milestone_tag_32600")));
