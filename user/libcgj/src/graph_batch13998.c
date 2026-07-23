/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13998: milestone 14000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_14000(void);
 *     - Returns the milestone tag for the milestone 14000 continuum
 *       (always 14000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_14000  (alias)
 *   __libcgj_batch13998_marker = "libcgj-batch13998"
 *
 * Milestone 14000 exclusive continuum CREATE-ONLY (13991-14000). Unique
 * gj_milestone_tag_14000 surface only; no multi-def. Distinct from
 * gj_graph_milestone_14000 (batch14000), gj_milestone_tag_13000
 * (batch12998), gj_milestone_tag_12900 (batch12898), and
 * gj_milestone_tag_12800 (batch12798). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13998_marker[] = "libcgj-batch13998";

/* Soft milestone tag for wave 14000. */
#define B13998_MILESTONE_TAG  14000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13998_tag(void)
{
	return B13998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_14000 - report milestone tag for wave 14000.
 *
 * Always returns 14000u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_14000(void)
{
	(void)NULL;
	return b13998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_14000(void)
    __attribute__((alias("gj_milestone_tag_14000")));
