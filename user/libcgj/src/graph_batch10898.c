/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10898: milestone 10900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_10900(void);
 *     - Returns the milestone tag for the milestone 10900 continuum
 *       (always 10900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_10900  (alias)
 *   __libcgj_batch10898_marker = "libcgj-batch10898"
 *
 * Milestone 10900 exclusive continuum CREATE-ONLY (10891-10900). Unique
 * gj_milestone_tag_10900 surface only; no multi-def. Distinct from
 * gj_graph_milestone_10900 (batch10900), gj_milestone_tag_10800
 * (batch10798), gj_milestone_tag_10700 (batch10698), and
 * gj_milestone_tag_10600 (batch10598). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10898_marker[] = "libcgj-batch10898";

/* Soft milestone tag for wave 10900. */
#define B10898_MILESTONE_TAG  10900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10898_tag(void)
{
	return B10898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_10900 - report milestone tag for wave 10900.
 *
 * Always returns 10900u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_10900(void)
{
	(void)NULL;
	return b10898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_10900(void)
    __attribute__((alias("gj_milestone_tag_10900")));
