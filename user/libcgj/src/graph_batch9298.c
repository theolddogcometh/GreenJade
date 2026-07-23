/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9298: milestone 9300 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_9300(void);
 *     - Returns the milestone fourCC tag for the milestone 9300
 *       continuum (always 0x4d393330, fourCC "M930"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_9300  (alias)
 *   __libcgj_batch9298_marker = "libcgj-batch9298"
 *
 * Milestone 9300 exclusive continuum CREATE-ONLY (9291-9300). Unique
 * gj_milestone_tag_9300 surface only; no multi-def. Distinct from
 * gj_graph_milestone_9300 (batch9300), gj_milestone_tag_9200
 * (batch9198), gj_milestone_tag_9100 (batch9098), and
 * gj_milestone_tag_9000 (batch8998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9298_marker[] = "libcgj-batch9298";

/* Soft "M930" fourCC-style milestone tag for wave 9300. */
#define B9298_MILESTONE_TAG  0x4d393330u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9298_tag(void)
{
	return B9298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_9300 - report milestone fourCC tag for wave 9300.
 *
 * Always returns 0x4d393330 ("M930"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_9300(void)
{
	(void)NULL;
	return b9298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_9300(void)
    __attribute__((alias("gj_milestone_tag_9300")));
