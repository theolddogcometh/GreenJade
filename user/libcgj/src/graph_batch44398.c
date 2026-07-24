/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44398: milestone 44400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_44400(void);
 *     - Returns the milestone tag for the milestone 44400 continuum
 *       (always 44400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_44400  (alias)
 *   __libcgj_batch44398_marker = "libcgj-batch44398"
 *
 * Milestone 44400 exclusive continuum CREATE-ONLY (44391-44400). Unique
 * gj_milestone_tag_44400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_44300 / gj_milestone_tag_44200 / gj_milestone_tag_44100,
 * gj_graph_milestone_44400 (batch44400), and gj_continuum_wave_44400
 * (batch44399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (44391–44400): host soft probes
 * (cgj_soft_milestone_44400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=44400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 44398. */
const char __libcgj_batch44398_marker[] = "libcgj-batch44398";

/* Soft milestone tag for wave 44400 (product identity = 44400). */
#define B44398_MILESTONE_TAG  44400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44398_tag(void)
{
	return B44398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_44400 - report soft milestone product tag for wave 44400.
 *
 * Always returns 44400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 44400.
 */
uint32_t
gj_milestone_tag_44400(void)
{
	(void)NULL;
	return b44398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_44400(void)
    __attribute__((alias("gj_milestone_tag_44400")));
