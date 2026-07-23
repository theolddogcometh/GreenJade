/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11998: milestone 12000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_12000(void);
 *     - Returns the milestone tag for the milestone 12000 continuum
 *       (always 12000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_12000  (alias)
 *   __libcgj_batch11998_marker = "libcgj-batch11998"
 *
 * Milestone 12000 exclusive continuum CREATE-ONLY (11991-12000). Unique
 * gj_milestone_tag_12000 surface only; no multi-def. Distinct from
 * gj_graph_milestone_12000 (batch12000), gj_milestone_tag_11900
 * (batch11898), gj_milestone_tag_11800 (batch11798), and
 * gj_milestone_tag_11700 (batch11698). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11998_marker[] = "libcgj-batch11998";

/* Soft milestone tag for wave 12000. */
#define B11998_MILESTONE_TAG  12000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11998_tag(void)
{
	return B11998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_12000 - report milestone tag for wave 12000.
 *
 * Always returns 12000u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_12000(void)
{
	(void)NULL;
	return b11998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_12000(void)
    __attribute__((alias("gj_milestone_tag_12000")));
