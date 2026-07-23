/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13898: milestone 13900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_13900(void);
 *     - Returns the milestone tag for the milestone 13900 continuum
 *       (always 13900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_13900  (alias)
 *   __libcgj_batch13898_marker = "libcgj-batch13898"
 *
 * Milestone 13900 exclusive continuum CREATE-ONLY (13891-13900). Unique
 * gj_milestone_tag_13900 surface only; no multi-def. Distinct from
 * gj_graph_milestone_13900 (batch13900), gj_milestone_tag_13800
 * (batch13798), gj_milestone_tag_13700 (batch13698), and
 * gj_milestone_tag_13600 (batch13598). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13898_marker[] = "libcgj-batch13898";

/* Soft milestone tag for wave 13900. */
#define B13898_MILESTONE_TAG  13900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13898_tag(void)
{
	return B13898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_13900 - report milestone tag for wave 13900.
 *
 * Always returns 13900u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_13900(void)
{
	(void)NULL;
	return b13898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_13900(void)
    __attribute__((alias("gj_milestone_tag_13900")));
