/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9398: milestone 9400 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_9400(void);
 *     - Returns the milestone fourCC tag for the milestone 9400
 *       continuum (always 0x4d393430, fourCC "M940"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_9400  (alias)
 *   __libcgj_batch9398_marker = "libcgj-batch9398"
 *
 * Milestone 9400 exclusive continuum CREATE-ONLY (9391-9400). Unique
 * gj_milestone_tag_9400 surface only; no multi-def. Distinct from
 * gj_graph_milestone_9400 (batch9400), gj_milestone_tag_9300
 * (batch9298), gj_milestone_tag_9200 (batch9198), and
 * gj_milestone_tag_9100 (batch9098). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9398_marker[] = "libcgj-batch9398";

/* Soft "M940" fourCC-style milestone tag for wave 9400. */
#define B9398_MILESTONE_TAG  0x4d393430u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9398_tag(void)
{
	return B9398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_9400 - report milestone fourCC tag for wave 9400.
 *
 * Always returns 0x4d393430 ("M940"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_9400(void)
{
	(void)NULL;
	return b9398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_9400(void)
    __attribute__((alias("gj_milestone_tag_9400")));
