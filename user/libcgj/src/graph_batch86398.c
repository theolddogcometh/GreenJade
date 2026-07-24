/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch86398: milestone 86400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_86400(void);
 *     - Returns the milestone tag for the milestone 86400 continuum
 *       (always 86400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_86400  (alias)
 *   __libcgj_batch86398_marker = "libcgj-batch86398"
 *
 * Milestone 86400 exclusive continuum CREATE-ONLY (25891-86400). Unique
 * gj_milestone_tag_86400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_86400 (batch86400), and gj_continuum_wave_86400
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–86400): host soft probes
 * (cgj_soft_milestone_86400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=86400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 86398. */
const char __libcgj_batch86398_marker[] = "libcgj-batch86398";

/* Soft milestone tag for wave 86400 (product identity = 86400). */
#define B86398_MILESTONE_TAG  86400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b86398_tag(void)
{
	return B86398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_86400 - report soft milestone product tag for wave 86400.
 *
 * Always returns 86400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 86400.
 */
uint32_t
gj_milestone_tag_86400(void)
{
	(void)NULL;
	return b86398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_86400(void)
    __attribute__((alias("gj_milestone_tag_86400")));
