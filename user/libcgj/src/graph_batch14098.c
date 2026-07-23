/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14098: milestone 14100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_14100(void);
 *     - Returns the milestone tag for the milestone 14100 continuum
 *       (always 14100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_14100  (alias)
 *   __libcgj_batch14098_marker = "libcgj-batch14098"
 *
 * Milestone 14100 exclusive continuum CREATE-ONLY (14091-14100). Unique
 * gj_milestone_tag_14100 surface only; no multi-def. Distinct from
 * gj_graph_milestone_14100 (batch14100), gj_milestone_tag_14000
 * (batch13998), gj_milestone_tag_13000 (batch12998), and
 * gj_milestone_tag_12900 (batch12898). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14098_marker[] = "libcgj-batch14098";

/* Soft milestone tag for wave 14100. */
#define B14098_MILESTONE_TAG  14100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14098_tag(void)
{
	return B14098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_14100 - report milestone tag for wave 14100.
 *
 * Always returns 14100u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_14100(void)
{
	(void)NULL;
	return b14098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_14100(void)
    __attribute__((alias("gj_milestone_tag_14100")));
