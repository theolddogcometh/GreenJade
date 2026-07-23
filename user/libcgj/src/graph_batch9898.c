/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9898: milestone 9900 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_9900(void);
 *     - Returns the milestone fourCC tag for the milestone 9900
 *       continuum (always 0x4d393930, fourCC "M990"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_9900  (alias)
 *   __libcgj_batch9898_marker = "libcgj-batch9898"
 *
 * Milestone 9900 exclusive continuum CREATE-ONLY (9891-9900). Unique
 * gj_milestone_tag_9900 surface only; no multi-def. Distinct from
 * gj_graph_milestone_9900 (batch9900), gj_milestone_tag_9800
 * (batch9798), gj_milestone_tag_9700 (batch9698), and
 * gj_milestone_tag_9600 (batch9598). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9898_marker[] = "libcgj-batch9898";

/* Soft "M990" fourCC-style milestone tag for wave 9900. */
#define B9898_MILESTONE_TAG  0x4d393930u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9898_tag(void)
{
	return B9898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_9900 - report milestone fourCC tag for wave 9900.
 *
 * Always returns 0x4d393930 ("M990"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_9900(void)
{
	(void)NULL;
	return b9898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_9900(void)
    __attribute__((alias("gj_milestone_tag_9900")));
