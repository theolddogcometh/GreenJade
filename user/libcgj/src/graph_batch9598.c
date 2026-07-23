/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9598: milestone 9600 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_9600(void);
 *     - Returns the milestone fourCC tag for the milestone 9600
 *       continuum (always 0x4d393630, fourCC "M960"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_9600  (alias)
 *   __libcgj_batch9598_marker = "libcgj-batch9598"
 *
 * Milestone 9600 exclusive continuum CREATE-ONLY (9591-9600). Unique
 * gj_milestone_tag_9600 surface only; no multi-def. Distinct from
 * gj_graph_milestone_9600 (batch9600), gj_milestone_tag_9500
 * (batch9498), gj_milestone_tag_9400 (batch9398), and
 * gj_milestone_tag_9300 (batch9298). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9598_marker[] = "libcgj-batch9598";

/* Soft "M960" fourCC-style milestone tag for wave 9600. */
#define B9598_MILESTONE_TAG  0x4d393630u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9598_tag(void)
{
	return B9598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_9600 - report milestone fourCC tag for wave 9600.
 *
 * Always returns 0x4d393630 ("M960"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_9600(void)
{
	(void)NULL;
	return b9598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_9600(void)
    __attribute__((alias("gj_milestone_tag_9600")));
