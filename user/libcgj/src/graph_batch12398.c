/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12398: milestone 12400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_12400(void);
 *     - Returns the milestone tag for the milestone 12400 continuum
 *       (always 12400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_12400  (alias)
 *   __libcgj_batch12398_marker = "libcgj-batch12398"
 *
 * Milestone 12400 exclusive continuum CREATE-ONLY (12391-12400). Unique
 * gj_milestone_tag_12400 surface only; no multi-def. Distinct from
 * gj_graph_milestone_12400 (batch12400), gj_milestone_tag_12300
 * (batch12298), gj_milestone_tag_12200 (batch12198), and
 * gj_milestone_tag_12100 (batch12098). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12398_marker[] = "libcgj-batch12398";

/* Soft milestone tag for wave 12400. */
#define B12398_MILESTONE_TAG  12400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12398_tag(void)
{
	return B12398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_12400 - report milestone tag for wave 12400.
 *
 * Always returns 12400u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_12400(void)
{
	(void)NULL;
	return b12398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_12400(void)
    __attribute__((alias("gj_milestone_tag_12400")));
