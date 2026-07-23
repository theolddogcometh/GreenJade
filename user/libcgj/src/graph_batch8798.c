/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8798: milestone 8800 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_8800(void);
 *     - Returns the milestone fourCC tag for the milestone 8800
 *       continuum (always 0x4d383830, fourCC "M880"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_8800  (alias)
 *   __libcgj_batch8798_marker = "libcgj-batch8798"
 *
 * Milestone 8800 exclusive continuum CREATE-ONLY (8791-8800). Unique
 * gj_milestone_tag_8800 surface only; no multi-def. Distinct from
 * gj_graph_milestone_8800 (batch8800), gj_milestone_tag_8700
 * (batch8698), gj_milestone_tag_8600 (batch8598), and
 * gj_milestone_tag_8500 (batch8498). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8798_marker[] = "libcgj-batch8798";

/* Soft "M880" fourCC-style milestone tag for wave 8800. */
#define B8798_MILESTONE_TAG  0x4d383830u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8798_tag(void)
{
	return B8798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_8800 - report milestone fourCC tag for wave 8800.
 *
 * Always returns 0x4d383830 ("M880"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_8800(void)
{
	(void)NULL;
	return b8798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_8800(void)
    __attribute__((alias("gj_milestone_tag_8800")));
