/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8698: milestone 8700 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_8700(void);
 *     - Returns the milestone fourCC tag for the milestone 8700
 *       continuum (always 0x4d383730, fourCC "M870"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_8700  (alias)
 *   __libcgj_batch8698_marker = "libcgj-batch8698"
 *
 * Milestone 8700 exclusive continuum CREATE-ONLY (8691-8700). Unique
 * gj_milestone_tag_8700 surface only; no multi-def. Distinct from
 * gj_graph_milestone_8700 (batch8700), gj_milestone_tag_8600
 * (batch8598), gj_milestone_tag_8500 (batch8498), and
 * gj_milestone_tag_8400 (batch8398). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8698_marker[] = "libcgj-batch8698";

/* Soft "M870" fourCC-style milestone tag for wave 8700. */
#define B8698_MILESTONE_TAG  0x4d383730u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8698_tag(void)
{
	return B8698_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_8700 - report milestone fourCC tag for wave 8700.
 *
 * Always returns 0x4d383730 ("M870"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_8700(void)
{
	(void)NULL;
	return b8698_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_8700(void)
    __attribute__((alias("gj_milestone_tag_8700")));
