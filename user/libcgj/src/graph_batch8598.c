/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8598: milestone 8600 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_8600(void);
 *     - Returns the milestone fourCC tag for the milestone 8600
 *       continuum (always 0x4d383630, fourCC "M860"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_8600  (alias)
 *   __libcgj_batch8598_marker = "libcgj-batch8598"
 *
 * Milestone 8600 exclusive continuum CREATE-ONLY (8591-8600). Unique
 * gj_milestone_tag_8600 surface only; no multi-def. Distinct from
 * gj_graph_milestone_8600 (batch8600), gj_milestone_tag_8500
 * (batch8498), gj_milestone_tag_8400 (batch8398), and
 * gj_milestone_tag_8300 (batch8298). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8598_marker[] = "libcgj-batch8598";

/* Soft "M860" fourCC-style milestone tag for wave 8600. */
#define B8598_MILESTONE_TAG  0x4d383630u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8598_tag(void)
{
	return B8598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_8600 - report milestone fourCC tag for wave 8600.
 *
 * Always returns 0x4d383630 ("M860"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_8600(void)
{
	(void)NULL;
	return b8598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_8600(void)
    __attribute__((alias("gj_milestone_tag_8600")));
