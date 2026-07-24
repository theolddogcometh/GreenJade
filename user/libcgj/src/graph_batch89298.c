/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch89298: milestone 89300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_89300(void);
 *     - Returns the milestone tag for the milestone 89300 continuum
 *       (always 89300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_89300  (alias)
 *   __libcgj_batch89298_marker = "libcgj-batch89298"
 *
 * Milestone 89300 exclusive continuum CREATE-ONLY (25891-89300). Unique
 * gj_milestone_tag_89300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_89300 (batch89300), and gj_continuum_wave_89300
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–89300): host soft probes
 * (cgj_soft_milestone_89300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=89300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 89298. */
const char __libcgj_batch89298_marker[] = "libcgj-batch89298";

/* Soft milestone tag for wave 89300 (product identity = 89300). */
#define B89298_MILESTONE_TAG  89300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b89298_tag(void)
{
	return B89298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_89300 - report soft milestone product tag for wave 89300.
 *
 * Always returns 89300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 89300.
 */
uint32_t
gj_milestone_tag_89300(void)
{
	(void)NULL;
	return b89298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_89300(void)
    __attribute__((alias("gj_milestone_tag_89300")));
