/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch66398: milestone 66400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_66400(void);
 *     - Returns the milestone tag for the milestone 66400 continuum
 *       (always 66400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_66400  (alias)
 *   __libcgj_batch66398_marker = "libcgj-batch66398"
 *
 * Milestone 66400 exclusive continuum CREATE-ONLY (25891-66400). Unique
 * gj_milestone_tag_66400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_66400 (batch66400), and gj_continuum_wave_66400
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–66400): host soft probes
 * (cgj_soft_milestone_66400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=66400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 66398. */
const char __libcgj_batch66398_marker[] = "libcgj-batch66398";

/* Soft milestone tag for wave 66400 (product identity = 66400). */
#define B66398_MILESTONE_TAG  66400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b66398_tag(void)
{
	return B66398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_66400 - report soft milestone product tag for wave 66400.
 *
 * Always returns 66400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 66400.
 */
uint32_t
gj_milestone_tag_66400(void)
{
	(void)NULL;
	return b66398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_66400(void)
    __attribute__((alias("gj_milestone_tag_66400")));
