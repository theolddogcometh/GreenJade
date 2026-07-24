/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36398: milestone 36400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_36400(void);
 *     - Returns the milestone tag for the milestone 36400 continuum
 *       (always 36400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_36400  (alias)
 *   __libcgj_batch36398_marker = "libcgj-batch36398"
 *
 * Milestone 36400 exclusive continuum CREATE-ONLY (36391-36400). Unique
 * gj_milestone_tag_36400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_36300 / gj_milestone_tag_36200 / gj_milestone_tag_36100,
 * gj_graph_milestone_36400 (batch36400), and gj_continuum_wave_36400
 * (batch36399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36391–36400): host soft probes
 * (cgj_soft_milestone_36400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36398. */
const char __libcgj_batch36398_marker[] = "libcgj-batch36398";

/* Soft milestone tag for wave 36400 (product identity = 36400). */
#define B36398_MILESTONE_TAG  36400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36398_tag(void)
{
	return B36398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_36400 - report soft milestone product tag for wave 36400.
 *
 * Always returns 36400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 36400.
 */
uint32_t
gj_milestone_tag_36400(void)
{
	(void)NULL;
	return b36398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_36400(void)
    __attribute__((alias("gj_milestone_tag_36400")));
