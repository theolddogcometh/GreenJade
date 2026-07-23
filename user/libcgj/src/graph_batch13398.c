/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13398: milestone 13400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_13400(void);
 *     - Returns the milestone tag for the milestone 13400 continuum
 *       (always 13400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_13400  (alias)
 *   __libcgj_batch13398_marker = "libcgj-batch13398"
 *
 * Milestone 13400 exclusive continuum CREATE-ONLY (13391-13400). Unique
 * gj_milestone_tag_13400 surface only; no multi-def. Distinct from
 * gj_graph_milestone_13400 (batch13400), gj_milestone_tag_13300
 * (batch13298), gj_milestone_tag_13200 (batch13198), and
 * gj_milestone_tag_13100 (batch13098). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13398_marker[] = "libcgj-batch13398";

/* Soft milestone tag for wave 13400. */
#define B13398_MILESTONE_TAG  13400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13398_tag(void)
{
	return B13398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_13400 - report milestone tag for wave 13400.
 *
 * Always returns 13400u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_13400(void)
{
	(void)NULL;
	return b13398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_13400(void)
    __attribute__((alias("gj_milestone_tag_13400")));
