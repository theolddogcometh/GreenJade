/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9798: milestone 9800 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_9800(void);
 *     - Returns the milestone fourCC tag for the milestone 9800
 *       continuum (always 0x4d393830, fourCC "M980"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_9800  (alias)
 *   __libcgj_batch9798_marker = "libcgj-batch9798"
 *
 * Milestone 9800 exclusive continuum CREATE-ONLY (9791-9800). Unique
 * gj_milestone_tag_9800 surface only; no multi-def. Distinct from
 * gj_graph_milestone_9800 (batch9800), gj_milestone_tag_9700
 * (batch9698), gj_milestone_tag_9600 (batch9598), and
 * gj_milestone_tag_9500 (batch9498). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9798_marker[] = "libcgj-batch9798";

/* Soft "M980" fourCC-style milestone tag for wave 9800. */
#define B9798_MILESTONE_TAG  0x4d393830u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9798_tag(void)
{
	return B9798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_9800 - report milestone fourCC tag for wave 9800.
 *
 * Always returns 0x4d393830 ("M980"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_9800(void)
{
	(void)NULL;
	return b9798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_9800(void)
    __attribute__((alias("gj_milestone_tag_9800")));
