/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13698: milestone 13700 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_13700(void);
 *     - Returns the milestone tag for the milestone 13700 continuum
 *       (always 13700u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_13700  (alias)
 *   __libcgj_batch13698_marker = "libcgj-batch13698"
 *
 * Milestone 13700 exclusive continuum CREATE-ONLY (13691-13700). Unique
 * gj_milestone_tag_13700 surface only; no multi-def. Distinct from
 * gj_graph_milestone_13700 (batch13700), gj_milestone_tag_13600
 * (batch13598), gj_milestone_tag_13500 (batch13498), and
 * gj_milestone_tag_13400 (batch13398). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13698_marker[] = "libcgj-batch13698";

/* Soft milestone tag for wave 13700. */
#define B13698_MILESTONE_TAG  13700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13698_tag(void)
{
	return B13698_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_13700 - report milestone tag for wave 13700.
 *
 * Always returns 13700u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_13700(void)
{
	(void)NULL;
	return b13698_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_13700(void)
    __attribute__((alias("gj_milestone_tag_13700")));
