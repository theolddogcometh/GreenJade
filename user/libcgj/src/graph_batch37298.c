/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37298: milestone 37300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_37300(void);
 *     - Returns the milestone tag for the milestone 37300 continuum
 *       (always 37300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_37300  (alias)
 *   __libcgj_batch37298_marker = "libcgj-batch37298"
 *
 * Milestone 37300 exclusive continuum CREATE-ONLY (37291-37300). Unique
 * gj_milestone_tag_37300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_37200 / gj_milestone_tag_37100 / gj_milestone_tag_37000,
 * gj_graph_milestone_37300 (batch37300), and gj_continuum_wave_37300
 * (batch37299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (37291–37300): host soft probes
 * (cgj_soft_milestone_37300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=37300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 37298. */
const char __libcgj_batch37298_marker[] = "libcgj-batch37298";

/* Soft milestone tag for wave 37300 (product identity = 37300). */
#define B37298_MILESTONE_TAG  37300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37298_tag(void)
{
	return B37298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_37300 - report soft milestone product tag for wave 37300.
 *
 * Always returns 37300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 37300.
 */
uint32_t
gj_milestone_tag_37300(void)
{
	(void)NULL;
	return b37298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_37300(void)
    __attribute__((alias("gj_milestone_tag_37300")));
