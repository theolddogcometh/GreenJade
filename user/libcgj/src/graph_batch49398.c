/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49398: milestone 49400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_49400(void);
 *     - Returns the milestone tag for the milestone 49400 continuum
 *       (always 49400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_49400  (alias)
 *   __libcgj_batch49398_marker = "libcgj-batch49398"
 *
 * Milestone 49400 exclusive continuum CREATE-ONLY (49391-49400). Unique
 * gj_milestone_tag_49400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_49300 / gj_milestone_tag_49200 / gj_milestone_tag_49100,
 * gj_graph_milestone_49400 (batch49400), and gj_continuum_wave_49400
 * (batch49399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (49391–49400): host soft probes
 * (cgj_soft_milestone_49400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=49400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 49398. */
const char __libcgj_batch49398_marker[] = "libcgj-batch49398";

/* Soft milestone tag for wave 49400 (product identity = 49400). */
#define B49398_MILESTONE_TAG  49400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49398_tag(void)
{
	return B49398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_49400 - report soft milestone product tag for wave 49400.
 *
 * Always returns 49400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 49400.
 */
uint32_t
gj_milestone_tag_49400(void)
{
	(void)NULL;
	return b49398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_49400(void)
    __attribute__((alias("gj_milestone_tag_49400")));
