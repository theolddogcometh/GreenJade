/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8298: milestone 8300 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_8300(void);
 *     - Returns the milestone fourCC tag for the milestone 8300
 *       continuum (always 0x4d383330, fourCC "M830"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_8300  (alias)
 *   __libcgj_batch8298_marker = "libcgj-batch8298"
 *
 * Milestone 8300 exclusive continuum CREATE-ONLY (8291-8300). Unique
 * gj_milestone_tag_8300 surface only; no multi-def. Distinct from
 * gj_graph_milestone_8300 (batch8300), gj_graph_milestone_8200
 * (batch8200), and earlier fourCC product tags. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8298_marker[] = "libcgj-batch8298";

/* Soft "M830" fourCC-style milestone tag for wave 8300. */
#define B8298_MILESTONE_TAG  0x4d383330u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8298_tag(void)
{
	return B8298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_8300 - report milestone fourCC tag for wave 8300.
 *
 * Always returns 0x4d383330 ("M830"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_8300(void)
{
	(void)NULL;
	return b8298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_8300(void)
    __attribute__((alias("gj_milestone_tag_8300")));
