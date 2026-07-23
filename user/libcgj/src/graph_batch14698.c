/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14698: milestone 14700 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_14700(void);
 *     - Returns the milestone tag for the milestone 14700 continuum
 *       (always 14700u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_14700  (alias)
 *   __libcgj_batch14698_marker = "libcgj-batch14698"
 *
 * Milestone 14700 exclusive continuum CREATE-ONLY (14691-14700). Unique surface
 * only; no multi-def. Distinct from gj_*_14600 / gj_*_14500 / gj_*_14400 milestone
 * surfaces and sibling 14700 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14698_marker[] = "libcgj-batch14698";

/* Soft milestone tag for wave 14700. */
#define B14698_MILESTONE_TAG  14700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14698_tag(void)
{
	return B14698_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_14700 - soft continuum surface
 *
 * Always returns 14700u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_milestone_tag_14700(void)
{
	(void)NULL;
	return b14698_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_14700(void)
    __attribute__((alias("gj_milestone_tag_14700")));
