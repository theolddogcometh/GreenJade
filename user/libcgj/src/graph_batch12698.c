/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12698: milestone 12700 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_12700(void);
 *     - Returns the milestone tag for the milestone 12700 continuum
 *       (always 12700u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_12700  (alias)
 *   __libcgj_batch12698_marker = "libcgj-batch12698"
 *
 * Milestone 12700 exclusive continuum CREATE-ONLY (12691-12700). Unique
 * gj_milestone_tag_12700 surface only; no multi-def. Distinct from
 * gj_graph_milestone_12700 (batch12700), gj_milestone_tag_12600
 * (batch12598), gj_milestone_tag_12500 (batch12498), and
 * gj_milestone_tag_12400 (batch12398). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12698_marker[] = "libcgj-batch12698";

/* Soft milestone tag for wave 12700. */
#define B12698_MILESTONE_TAG  12700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12698_tag(void)
{
	return B12698_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_12700 - report milestone tag for wave 12700.
 *
 * Always returns 12700u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_12700(void)
{
	(void)NULL;
	return b12698_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_12700(void)
    __attribute__((alias("gj_milestone_tag_12700")));
