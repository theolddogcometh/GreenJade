/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8998: milestone 9000 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_9000(void);
 *     - Returns the milestone fourCC tag for the milestone 9000
 *       continuum (always 0x4d393030, fourCC "M900"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_9000  (alias)
 *   __libcgj_batch8998_marker = "libcgj-batch8998"
 *
 * Milestone 9000 exclusive continuum CREATE-ONLY (8991-9000). Unique
 * gj_milestone_tag_9000 surface only; no multi-def. Distinct from
 * gj_graph_milestone_9000 (batch9000), gj_milestone_tag_8900
 * (batch8898), gj_milestone_tag_8800 (batch8798), and
 * gj_milestone_tag_8700 (batch8698). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8998_marker[] = "libcgj-batch8998";

/* Soft "M900" fourCC-style milestone tag for wave 9000. */
#define B8998_MILESTONE_TAG  0x4d393030u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8998_tag(void)
{
	return B8998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_9000 - report milestone fourCC tag for wave 9000.
 *
 * Always returns 0x4d393030 ("M900"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_9000(void)
{
	(void)NULL;
	return b8998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_9000(void)
    __attribute__((alias("gj_milestone_tag_9000")));
