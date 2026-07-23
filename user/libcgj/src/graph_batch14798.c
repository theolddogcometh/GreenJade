/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14798: milestone 14800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_14800(void);
 *     - Returns the milestone tag for the milestone 14800 continuum
 *       (always 14800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_14800  (alias)
 *   __libcgj_batch14798_marker = "libcgj-batch14798"
 *
 * Milestone 14800 exclusive continuum CREATE-ONLY (14791-14800). Unique surface
 * only; no multi-def. Distinct from gj_*_14700 / gj_*_14600 / gj_*_14500 milestone
 * surfaces and sibling 14800 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14798_marker[] = "libcgj-batch14798";

/* Soft milestone tag for wave 14800. */
#define B14798_MILESTONE_TAG  14800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14798_tag(void)
{
	return B14798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_14800 - soft continuum surface
 *
 * Always returns 14800u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_milestone_tag_14800(void)
{
	(void)NULL;
	return b14798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_14800(void)
    __attribute__((alias("gj_milestone_tag_14800")));
