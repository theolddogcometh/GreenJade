/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch45298: milestone 45300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_45300(void);
 *     - Returns the milestone tag for the milestone 45300 continuum
 *       (always 45300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_45300  (alias)
 *   __libcgj_batch45298_marker = "libcgj-batch45298"
 *
 * Milestone 45300 exclusive continuum CREATE-ONLY (45291-45300). Unique
 * gj_milestone_tag_45300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_45100 / gj_milestone_tag_45000 / gj_milestone_tag_44900,
 * gj_graph_milestone_45300 (batch45300), and gj_continuum_wave_45300
 * (batch45299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (45291–45300): host soft probes
 * (cgj_soft_milestone_45300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=45300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 45298. */
const char __libcgj_batch45298_marker[] = "libcgj-batch45298";

/* Soft milestone tag for wave 45300 (product identity = 45300). */
#define B45298_MILESTONE_TAG  45300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b45298_tag(void)
{
	return B45298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_45300 - report soft milestone product tag for wave 45300.
 *
 * Always returns 45300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 45300.
 */
uint32_t
gj_milestone_tag_45300(void)
{
	(void)NULL;
	return b45298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_45300(void)
    __attribute__((alias("gj_milestone_tag_45300")));
