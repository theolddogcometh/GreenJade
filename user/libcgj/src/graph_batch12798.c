/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12798: milestone 12800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_12800(void);
 *     - Returns the milestone tag for the milestone 12800 continuum
 *       (always 12800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_12800  (alias)
 *   __libcgj_batch12798_marker = "libcgj-batch12798"
 *
 * Milestone 12800 exclusive continuum CREATE-ONLY (12791-12800). Unique
 * gj_milestone_tag_12800 surface only; no multi-def. Distinct from
 * gj_graph_milestone_12800 (batch12800), gj_milestone_tag_12700
 * (batch12698), gj_milestone_tag_12600 (batch12598), and
 * gj_milestone_tag_12500 (batch12498). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12798_marker[] = "libcgj-batch12798";

/* Soft milestone tag for wave 12800. */
#define B12798_MILESTONE_TAG  12800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12798_tag(void)
{
	return B12798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_12800 - report milestone tag for wave 12800.
 *
 * Always returns 12800u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_12800(void)
{
	(void)NULL;
	return b12798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_12800(void)
    __attribute__((alias("gj_milestone_tag_12800")));
