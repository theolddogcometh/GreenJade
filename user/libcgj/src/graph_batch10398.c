/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10398: milestone 10400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_10400(void);
 *     - Returns the milestone tag for the milestone 10400 continuum
 *       (always 10400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_10400  (alias)
 *   __libcgj_batch10398_marker = "libcgj-batch10398"
 *
 * Milestone 10400 exclusive continuum CREATE-ONLY (10391-10400). Unique
 * gj_milestone_tag_10400 surface only; no multi-def. Distinct from
 * gj_graph_milestone_10400 (batch10400), gj_milestone_tag_10300
 * (batch10298), gj_milestone_tag_10200 (batch10198), and
 * gj_milestone_tag_10100 (batch10098). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10398_marker[] = "libcgj-batch10398";

/* Soft milestone tag for wave 10400. */
#define B10398_MILESTONE_TAG  10400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10398_tag(void)
{
	return B10398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_10400 - report milestone tag for wave 10400.
 *
 * Always returns 10400u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_10400(void)
{
	(void)NULL;
	return b10398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_10400(void)
    __attribute__((alias("gj_milestone_tag_10400")));
