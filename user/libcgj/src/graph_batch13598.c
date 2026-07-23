/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13598: milestone 13600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_13600(void);
 *     - Returns the milestone tag for the milestone 13600 continuum
 *       (always 13600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_13600  (alias)
 *   __libcgj_batch13598_marker = "libcgj-batch13598"
 *
 * Milestone 13600 exclusive continuum CREATE-ONLY (13591-13600). Unique
 * gj_milestone_tag_13600 surface only; no multi-def. Distinct from
 * gj_graph_milestone_13600 (batch13600), gj_milestone_tag_13500
 * (batch13498), gj_milestone_tag_13400 (batch13398), and
 * gj_milestone_tag_13300 (batch13298). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13598_marker[] = "libcgj-batch13598";

/* Soft milestone tag for wave 13600. */
#define B13598_MILESTONE_TAG  13600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13598_tag(void)
{
	return B13598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_13600 - report milestone tag for wave 13600.
 *
 * Always returns 13600u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_13600(void)
{
	(void)NULL;
	return b13598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_13600(void)
    __attribute__((alias("gj_milestone_tag_13600")));
