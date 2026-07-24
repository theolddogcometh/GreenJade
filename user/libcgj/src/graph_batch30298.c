/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30298: milestone 30300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_30300(void);
 *     - Returns the milestone tag for the milestone 30300 continuum
 *       (always 30300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_30300  (alias)
 *   __libcgj_batch30298_marker = "libcgj-batch30298"
 *
 * Milestone 30300 exclusive continuum CREATE-ONLY (30291-30300). Unique
 * gj_milestone_tag_30300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_30200 / gj_milestone_tag_30100 / gj_milestone_tag_30000,
 * gj_graph_milestone_30300 (batch30300), and gj_continuum_wave_30300
 * (batch30299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30291–30300): host soft probes
 * (cgj_soft_milestone_30300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30298. */
const char __libcgj_batch30298_marker[] = "libcgj-batch30298";

/* Soft milestone tag for wave 30300 (product identity = 30300). */
#define B30298_MILESTONE_TAG  30300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30298_tag(void)
{
	return B30298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_30300 - report soft milestone product tag for wave 30300.
 *
 * Always returns 30300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 30300.
 */
uint32_t
gj_milestone_tag_30300(void)
{
	(void)NULL;
	return b30298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_30300(void)
    __attribute__((alias("gj_milestone_tag_30300")));
