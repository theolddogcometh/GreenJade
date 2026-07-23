/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13098: milestone 13100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_13100(void);
 *     - Returns the milestone tag for the milestone 13100 continuum
 *       (always 13100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_13100  (alias)
 *   __libcgj_batch13098_marker = "libcgj-batch13098"
 *
 * Milestone 13100 exclusive continuum CREATE-ONLY (13091-13100). Unique
 * gj_milestone_tag_13100 surface only; no multi-def. Distinct from
 * gj_graph_milestone_13100 (batch13100), gj_milestone_tag_13000
 * (batch12998), gj_milestone_tag_12900 (batch12898), and
 * gj_milestone_tag_12800 (batch12798). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13098_marker[] = "libcgj-batch13098";

/* Soft milestone tag for wave 13100. */
#define B13098_MILESTONE_TAG  13100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13098_tag(void)
{
	return B13098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_13100 - report milestone tag for wave 13100.
 *
 * Always returns 13100u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_13100(void)
{
	(void)NULL;
	return b13098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_13100(void)
    __attribute__((alias("gj_milestone_tag_13100")));
