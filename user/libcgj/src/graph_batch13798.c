/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13798: milestone 13800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_13800(void);
 *     - Returns the milestone tag for the milestone 13800 continuum
 *       (always 13800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_13800  (alias)
 *   __libcgj_batch13798_marker = "libcgj-batch13798"
 *
 * Milestone 13800 exclusive continuum CREATE-ONLY (13791-13800). Unique
 * gj_milestone_tag_13800 surface only; no multi-def. Distinct from
 * gj_graph_milestone_13800 (batch13800), gj_milestone_tag_13700
 * (batch13698), gj_milestone_tag_13600 (batch13598), and
 * gj_milestone_tag_13500 (batch13498). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13798_marker[] = "libcgj-batch13798";

/* Soft milestone tag for wave 13800. */
#define B13798_MILESTONE_TAG  13800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13798_tag(void)
{
	return B13798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_13800 - report milestone tag for wave 13800.
 *
 * Always returns 13800u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_13800(void)
{
	(void)NULL;
	return b13798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_13800(void)
    __attribute__((alias("gj_milestone_tag_13800")));
