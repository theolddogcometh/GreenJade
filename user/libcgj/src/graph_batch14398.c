/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14398: milestone 14400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_14400(void);
 *     - Returns the milestone tag for the milestone 14400 continuum
 *       (always 14400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_14400  (alias)
 *   __libcgj_batch14398_marker = "libcgj-batch14398"
 *
 * Milestone 14400 exclusive continuum CREATE-ONLY (14391-14400). Unique
 * gj_milestone_tag_14400 surface only; no multi-def. Distinct from
 * gj_graph_milestone_14400 (batch14400), gj_milestone_tag_14000
 * (batch13998), gj_milestone_tag_13000 (batch12998), and
 * gj_milestone_tag_12900 (batch12898). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14398_marker[] = "libcgj-batch14398";

/* Soft milestone tag for wave 14400. */
#define B14398_MILESTONE_TAG  14400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14398_tag(void)
{
	return B14398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_14400 - report milestone tag for wave 14400.
 *
 * Always returns 14400u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_14400(void)
{
	(void)NULL;
	return b14398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_14400(void)
    __attribute__((alias("gj_milestone_tag_14400")));
