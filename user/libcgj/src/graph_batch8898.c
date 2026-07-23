/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8898: milestone 8900 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_8900(void);
 *     - Returns the milestone fourCC tag for the milestone 8900
 *       continuum (always 0x4d383930, fourCC "M890"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_8900  (alias)
 *   __libcgj_batch8898_marker = "libcgj-batch8898"
 *
 * Milestone 8900 exclusive continuum CREATE-ONLY (8891-8900). Unique
 * gj_milestone_tag_8900 surface only; no multi-def. Distinct from
 * gj_graph_milestone_8900 (batch8900), gj_milestone_tag_8800
 * (batch8798), gj_milestone_tag_8700 (batch8698), and
 * gj_milestone_tag_8600 (batch8598). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8898_marker[] = "libcgj-batch8898";

/* Soft "M890" fourCC-style milestone tag for wave 8900. */
#define B8898_MILESTONE_TAG  0x4d383930u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8898_tag(void)
{
	return B8898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_8900 - report milestone fourCC tag for wave 8900.
 *
 * Always returns 0x4d383930 ("M890"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_8900(void)
{
	(void)NULL;
	return b8898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_8900(void)
    __attribute__((alias("gj_milestone_tag_8900")));
