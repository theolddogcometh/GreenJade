/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13298: milestone 13300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_13300(void);
 *     - Returns the milestone tag for the milestone 13300 continuum
 *       (always 13300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_13300  (alias)
 *   __libcgj_batch13298_marker = "libcgj-batch13298"
 *
 * Milestone 13300 exclusive continuum CREATE-ONLY (13291-13300). Unique
 * gj_milestone_tag_13300 surface only; no multi-def. Distinct from
 * gj_graph_milestone_13300 (batch13300), gj_milestone_tag_13200
 * (batch13198), gj_milestone_tag_13100 (batch13098), and
 * gj_milestone_tag_13000 (batch12998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13298_marker[] = "libcgj-batch13298";

/* Soft milestone tag for wave 13300. */
#define B13298_MILESTONE_TAG  13300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13298_tag(void)
{
	return B13298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_13300 - report milestone tag for wave 13300.
 *
 * Always returns 13300u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_13300(void)
{
	(void)NULL;
	return b13298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_13300(void)
    __attribute__((alias("gj_milestone_tag_13300")));
