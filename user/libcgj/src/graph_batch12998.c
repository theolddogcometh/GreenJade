/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12998: milestone 13000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_13000(void);
 *     - Returns the milestone tag for the milestone 13000 continuum
 *       (always 13000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_13000  (alias)
 *   __libcgj_batch12998_marker = "libcgj-batch12998"
 *
 * Milestone 13000 exclusive continuum CREATE-ONLY (12991-13000). Unique
 * gj_milestone_tag_13000 surface only; no multi-def. Distinct from
 * gj_graph_milestone_13000 (batch13000), gj_milestone_tag_12900
 * (batch12898), gj_milestone_tag_12800 (batch12798), and
 * gj_milestone_tag_12700 (batch12698). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12998_marker[] = "libcgj-batch12998";

/* Soft milestone tag for wave 13000. */
#define B12998_MILESTONE_TAG  13000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12998_tag(void)
{
	return B12998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_13000 - report milestone tag for wave 13000.
 *
 * Always returns 13000u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_13000(void)
{
	(void)NULL;
	return b12998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_13000(void)
    __attribute__((alias("gj_milestone_tag_13000")));
