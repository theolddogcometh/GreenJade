/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11298: milestone 11300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_11300(void);
 *     - Returns the milestone tag for the milestone 11300 continuum
 *       (always 11300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_11300  (alias)
 *   __libcgj_batch11298_marker = "libcgj-batch11298"
 *
 * Milestone 11300 exclusive continuum CREATE-ONLY (11291-11300). Unique
 * gj_milestone_tag_11300 surface only; no multi-def. Distinct from
 * gj_graph_milestone_11300 (batch11300), gj_milestone_tag_11200
 * (batch11198), gj_milestone_tag_11100 (batch11098), and
 * gj_milestone_tag_11000 (batch10998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11298_marker[] = "libcgj-batch11298";

/* Soft milestone tag for wave 11300. */
#define B11298_MILESTONE_TAG  11300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11298_tag(void)
{
	return B11298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_11300 - report milestone tag for wave 11300.
 *
 * Always returns 11300u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_11300(void)
{
	(void)NULL;
	return b11298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_11300(void)
    __attribute__((alias("gj_milestone_tag_11300")));
