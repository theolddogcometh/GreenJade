/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18298: milestone 18300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_18300(void);
 *     - Returns the milestone tag for the milestone 18300 continuum
 *       (always 18300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_18300  (alias)
 *   __libcgj_batch18298_marker = "libcgj-batch18298"
 *
 * Milestone 18300 exclusive continuum CREATE-ONLY (18291-18300). Unique
 * gj_milestone_tag_18300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_18100 / gj_milestone_tag_18000 / gj_milestone_tag_17900,
 * gj_graph_milestone_18300 (batch18300), and gj_continuum_wave_18300
 * (batch18299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18291–18300): host soft probes
 * (cgj_soft_milestone_18300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=18300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18298. */
const char __libcgj_batch18298_marker[] = "libcgj-batch18298";

/* Soft milestone tag for wave 18300 (product identity = 18300). */
#define B18298_MILESTONE_TAG  18300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18298_tag(void)
{
	return B18298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_18300 - report soft milestone product tag for wave 18300.
 *
 * Always returns 18300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 18300.
 */
uint32_t
gj_milestone_tag_18300(void)
{
	(void)NULL;
	return b18298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_18300(void)
    __attribute__((alias("gj_milestone_tag_18300")));
