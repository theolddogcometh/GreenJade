/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26398: milestone 26400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_26400(void);
 *     - Returns the milestone tag for the milestone 26400 continuum
 *       (always 26400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_26400  (alias)
 *   __libcgj_batch26398_marker = "libcgj-batch26398"
 *
 * Milestone 26400 exclusive continuum CREATE-ONLY (26391-26400). Unique
 * gj_milestone_tag_26400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_26300 / gj_milestone_tag_26200 / gj_milestone_tag_26100,
 * gj_graph_milestone_26400 (batch26400), and gj_continuum_wave_26400
 * (batch26399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26391–26400): host soft probes
 * (cgj_soft_milestone_26400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26398. */
const char __libcgj_batch26398_marker[] = "libcgj-batch26398";

/* Soft milestone tag for wave 26400 (product identity = 26400). */
#define B26398_MILESTONE_TAG  26400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26398_tag(void)
{
	return B26398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_26400 - report soft milestone product tag for wave 26400.
 *
 * Always returns 26400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 26400.
 */
uint32_t
gj_milestone_tag_26400(void)
{
	(void)NULL;
	return b26398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_26400(void)
    __attribute__((alias("gj_milestone_tag_26400")));
