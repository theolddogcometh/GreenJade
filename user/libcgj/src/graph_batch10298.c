/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10298: milestone 10300 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_10300(void);
 *     - Returns the milestone fourCC tag for the milestone 10300
 *       continuum (always 0x47313033, fourCC "G103"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_10300  (alias)
 *   __libcgj_batch10298_marker = "libcgj-batch10298"
 *
 * Milestone 10300 exclusive continuum CREATE-ONLY (10291-10300). Unique
 * gj_milestone_tag_10300 surface only; no multi-def. Distinct from
 * gj_graph_milestone_10300 (batch10300), gj_milestone_tag_10200
 * (batch10198), gj_milestone_tag_10100 (batch10098), and
 * gj_milestone_tag_10000 (batch9998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10298_marker[] = "libcgj-batch10298";

/* Soft "G103" fourCC-style milestone tag for wave 10300. */
#define B10298_MILESTONE_TAG  0x47313033u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10298_tag(void)
{
	return B10298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_10300 - report milestone fourCC tag for wave 10300.
 *
 * Always returns 0x47313033 ("G103"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_10300(void)
{
	(void)NULL;
	return b10298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_10300(void)
    __attribute__((alias("gj_milestone_tag_10300")));
