/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10098: milestone 10100 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_10100(void);
 *     - Returns the milestone fourCC tag for the milestone 10100
 *       continuum (always 0x4d313031, fourCC "M101"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_10100  (alias)
 *   __libcgj_batch10098_marker = "libcgj-batch10098"
 *
 * Milestone 10100 exclusive continuum CREATE-ONLY (10091-10100). Unique
 * gj_milestone_tag_10100 surface only; no multi-def. Distinct from
 * gj_graph_milestone_10100 (batch10100), gj_milestone_tag_9900
 * (batch9898), gj_milestone_tag_9100 (batch9098), and
 * gj_milestone_tag_9000 (batch8998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10098_marker[] = "libcgj-batch10098";

/* Soft "M101" fourCC-style milestone tag for wave 10100. */
#define B10098_MILESTONE_TAG  0x4d313031u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10098_tag(void)
{
	return B10098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_10100 - report milestone fourCC tag for wave 10100.
 *
 * Always returns 0x4d313031 ("M101"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_10100(void)
{
	(void)NULL;
	return b10098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_10100(void)
    __attribute__((alias("gj_milestone_tag_10100")));
