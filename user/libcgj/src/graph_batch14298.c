/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14298: milestone 14300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_14300(void);
 *     - Returns the milestone tag for the milestone 14300 continuum
 *       (always 14300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_14300  (alias)
 *   __libcgj_batch14298_marker = "libcgj-batch14298"
 *
 * Milestone 14300 exclusive continuum CREATE-ONLY (14291-14300). Unique
 * gj_milestone_tag_14300 surface only; no multi-def. Distinct from
 * gj_graph_milestone_14300 (batch14300), gj_milestone_tag_14200
 * (batch14198), gj_milestone_tag_14100 (batch14098), and
 * gj_milestone_tag_14000 (batch13998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14298_marker[] = "libcgj-batch14298";

/* Soft milestone tag for wave 14300. */
#define B14298_MILESTONE_TAG  14300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14298_tag(void)
{
	return B14298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_14300 - report milestone tag for wave 14300.
 *
 * Always returns 14300u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_14300(void)
{
	(void)NULL;
	return b14298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_14300(void)
    __attribute__((alias("gj_milestone_tag_14300")));
