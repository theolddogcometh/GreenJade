/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13498: milestone 13500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_13500(void);
 *     - Returns the milestone tag for the milestone 13500 continuum
 *       (always 13500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_13500  (alias)
 *   __libcgj_batch13498_marker = "libcgj-batch13498"
 *
 * Milestone 13500 exclusive continuum CREATE-ONLY (13491-13500). Unique
 * gj_milestone_tag_13500 surface only; no multi-def. Distinct from
 * gj_graph_milestone_13500 (batch13500), gj_milestone_tag_13400
 * (batch13398), gj_milestone_tag_13300 (batch13298), and
 * gj_milestone_tag_13200 (batch13198). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13498_marker[] = "libcgj-batch13498";

/* Soft milestone tag for wave 13500. */
#define B13498_MILESTONE_TAG  13500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13498_tag(void)
{
	return B13498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_13500 - report milestone tag for wave 13500.
 *
 * Always returns 13500u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_13500(void)
{
	(void)NULL;
	return b13498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_13500(void)
    __attribute__((alias("gj_milestone_tag_13500")));
