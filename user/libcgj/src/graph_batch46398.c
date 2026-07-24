/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46398: milestone 46400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_46400(void);
 *     - Returns the milestone tag for the milestone 46400 continuum
 *       (always 46400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_46400  (alias)
 *   __libcgj_batch46398_marker = "libcgj-batch46398"
 *
 * Milestone 46400 exclusive continuum CREATE-ONLY (46391-46400). Unique
 * gj_milestone_tag_46400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_46300 / gj_milestone_tag_46200 / gj_milestone_tag_46100,
 * gj_graph_milestone_46400 (batch46400), and gj_continuum_wave_46400
 * (batch46399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (46391–46400): host soft probes
 * (cgj_soft_milestone_46400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=46400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 46398. */
const char __libcgj_batch46398_marker[] = "libcgj-batch46398";

/* Soft milestone tag for wave 46400 (product identity = 46400). */
#define B46398_MILESTONE_TAG  46400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46398_tag(void)
{
	return B46398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_46400 - report soft milestone product tag for wave 46400.
 *
 * Always returns 46400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 46400.
 */
uint32_t
gj_milestone_tag_46400(void)
{
	(void)NULL;
	return b46398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_46400(void)
    __attribute__((alias("gj_milestone_tag_46400")));
