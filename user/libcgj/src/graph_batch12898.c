/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12898: milestone 12900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_12900(void);
 *     - Returns the milestone tag for the milestone 12900 continuum
 *       (always 12900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_12900  (alias)
 *   __libcgj_batch12898_marker = "libcgj-batch12898"
 *
 * Milestone 12900 exclusive continuum CREATE-ONLY (12891-12900). Unique
 * gj_milestone_tag_12900 surface only; no multi-def. Distinct from
 * gj_graph_milestone_12900 (batch12900), gj_milestone_tag_12800
 * (batch12798), gj_milestone_tag_12700 (batch12698), and
 * gj_milestone_tag_12600 (batch12598). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12898_marker[] = "libcgj-batch12898";

/* Soft milestone tag for wave 12900. */
#define B12898_MILESTONE_TAG  12900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12898_tag(void)
{
	return B12898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_12900 - report milestone tag for wave 12900.
 *
 * Always returns 12900u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_12900(void)
{
	(void)NULL;
	return b12898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_12900(void)
    __attribute__((alias("gj_milestone_tag_12900")));
