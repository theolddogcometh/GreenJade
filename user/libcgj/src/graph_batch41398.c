/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch41398: milestone 41400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_41400(void);
 *     - Returns the milestone tag for the milestone 41400 continuum
 *       (always 41400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_41400  (alias)
 *   __libcgj_batch41398_marker = "libcgj-batch41398"
 *
 * Milestone 41400 exclusive continuum CREATE-ONLY (41391-41400). Unique
 * gj_milestone_tag_41400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_41300 / gj_milestone_tag_41200 / gj_milestone_tag_41100,
 * gj_graph_milestone_41400 (batch41400), and gj_continuum_wave_41400
 * (batch41399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (41391–41400): host soft probes
 * (cgj_soft_milestone_41400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=41400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 41398. */
const char __libcgj_batch41398_marker[] = "libcgj-batch41398";

/* Soft milestone tag for wave 41400 (product identity = 41400). */
#define B41398_MILESTONE_TAG  41400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b41398_tag(void)
{
	return B41398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_41400 - report soft milestone product tag for wave 41400.
 *
 * Always returns 41400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 41400.
 */
uint32_t
gj_milestone_tag_41400(void)
{
	(void)NULL;
	return b41398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_41400(void)
    __attribute__((alias("gj_milestone_tag_41400")));
