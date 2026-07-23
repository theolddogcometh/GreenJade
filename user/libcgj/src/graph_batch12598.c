/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12598: milestone 12600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_12600(void);
 *     - Returns the milestone tag for the milestone 12600 continuum
 *       (always 12600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_12600  (alias)
 *   __libcgj_batch12598_marker = "libcgj-batch12598"
 *
 * Milestone 12600 exclusive continuum CREATE-ONLY (12591-12600). Unique
 * gj_milestone_tag_12600 surface only; no multi-def. Distinct from
 * gj_graph_milestone_12600 (batch12600), gj_milestone_tag_12500
 * (batch12498), gj_milestone_tag_12400 (batch12398), and
 * gj_milestone_tag_12300 (batch12298). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12598_marker[] = "libcgj-batch12598";

/* Soft milestone tag for wave 12600. */
#define B12598_MILESTONE_TAG  12600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12598_tag(void)
{
	return B12598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_12600 - report milestone tag for wave 12600.
 *
 * Always returns 12600u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_12600(void)
{
	(void)NULL;
	return b12598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_12600(void)
    __attribute__((alias("gj_milestone_tag_12600")));
