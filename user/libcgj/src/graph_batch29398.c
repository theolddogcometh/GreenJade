/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29398: milestone 29400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_29400(void);
 *     - Returns the milestone tag for the milestone 29400 continuum
 *       (always 29400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_29400  (alias)
 *   __libcgj_batch29398_marker = "libcgj-batch29398"
 *
 * Milestone 29400 exclusive continuum CREATE-ONLY (29391-29400). Unique
 * gj_milestone_tag_29400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_29300 / gj_milestone_tag_29200 / gj_milestone_tag_29100,
 * gj_graph_milestone_29400 (batch29400), and gj_continuum_wave_29400
 * (batch29399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (29391–29400): host soft probes
 * (cgj_soft_milestone_29400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=29400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 29398. */
const char __libcgj_batch29398_marker[] = "libcgj-batch29398";

/* Soft milestone tag for wave 29400 (product identity = 29400). */
#define B29398_MILESTONE_TAG  29400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29398_tag(void)
{
	return B29398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_29400 - report soft milestone product tag for wave 29400.
 *
 * Always returns 29400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 29400.
 */
uint32_t
gj_milestone_tag_29400(void)
{
	(void)NULL;
	return b29398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_29400(void)
    __attribute__((alias("gj_milestone_tag_29400")));
