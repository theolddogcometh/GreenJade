/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12098: milestone 12100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_12100(void);
 *     - Returns the milestone tag for the milestone 12100 continuum
 *       (always 12100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_12100  (alias)
 *   __libcgj_batch12098_marker = "libcgj-batch12098"
 *
 * Milestone 12100 exclusive continuum CREATE-ONLY (12091-12100). Unique
 * gj_milestone_tag_12100 surface only; no multi-def. Distinct from
 * gj_graph_milestone_12100 (batch12100), gj_milestone_tag_12000
 * (batch11998), gj_milestone_tag_11900 (batch11898), and
 * gj_milestone_tag_11800 (batch11798). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12098_marker[] = "libcgj-batch12098";

/* Soft milestone tag for wave 12100. */
#define B12098_MILESTONE_TAG  12100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12098_tag(void)
{
	return B12098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_12100 - report milestone tag for wave 12100.
 *
 * Always returns 12100u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_12100(void)
{
	(void)NULL;
	return b12098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_12100(void)
    __attribute__((alias("gj_milestone_tag_12100")));
