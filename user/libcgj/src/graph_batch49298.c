/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49298: milestone 49300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_49300(void);
 *     - Returns the milestone tag for the milestone 49300 continuum
 *       (always 49300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_49300  (alias)
 *   __libcgj_batch49298_marker = "libcgj-batch49298"
 *
 * Milestone 49300 exclusive continuum CREATE-ONLY (49291-49300). Unique
 * gj_milestone_tag_49300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_49200 / gj_milestone_tag_49100 / gj_milestone_tag_49000,
 * gj_graph_milestone_49300 (batch49300), and gj_continuum_wave_49300
 * (batch49299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (49291–49300): host soft probes
 * (cgj_soft_milestone_49300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=49300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 49298. */
const char __libcgj_batch49298_marker[] = "libcgj-batch49298";

/* Soft milestone tag for wave 49300 (product identity = 49300). */
#define B49298_MILESTONE_TAG  49300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49298_tag(void)
{
	return B49298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_49300 - report soft milestone product tag for wave 49300.
 *
 * Always returns 49300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 49300.
 */
uint32_t
gj_milestone_tag_49300(void)
{
	(void)NULL;
	return b49298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_49300(void)
    __attribute__((alias("gj_milestone_tag_49300")));
