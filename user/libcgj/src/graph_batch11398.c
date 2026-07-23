/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11398: milestone 11400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_11400(void);
 *     - Returns the milestone tag for the milestone 11400 continuum
 *       (always 11400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_11400  (alias)
 *   __libcgj_batch11398_marker = "libcgj-batch11398"
 *
 * Milestone 11400 exclusive continuum CREATE-ONLY (11391-11400). Unique
 * gj_milestone_tag_11400 surface only; no multi-def. Distinct from
 * gj_graph_milestone_11400 (batch11400), gj_milestone_tag_11300
 * (batch11298), gj_milestone_tag_11200 (batch11198), and
 * gj_milestone_tag_11100 (batch11098). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11398_marker[] = "libcgj-batch11398";

/* Soft milestone tag for wave 11400. */
#define B11398_MILESTONE_TAG  11400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11398_tag(void)
{
	return B11398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_11400 - report milestone tag for wave 11400.
 *
 * Always returns 11400u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_11400(void)
{
	(void)NULL;
	return b11398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_11400(void)
    __attribute__((alias("gj_milestone_tag_11400")));
