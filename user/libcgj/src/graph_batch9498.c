/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9498: milestone 9500 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_9500(void);
 *     - Returns the milestone fourCC tag for the milestone 9500
 *       continuum (always 0x4d393530, fourCC "M950"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_9500  (alias)
 *   __libcgj_batch9498_marker = "libcgj-batch9498"
 *
 * Milestone 9500 exclusive continuum CREATE-ONLY (9491-9500). Unique
 * gj_milestone_tag_9500 surface only; no multi-def. Distinct from
 * gj_graph_milestone_9500 (batch9500), gj_milestone_tag_9400
 * (batch9398), gj_milestone_tag_9300 (batch9298), and
 * gj_milestone_tag_9200 (batch9198). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9498_marker[] = "libcgj-batch9498";

/* Soft "M950" fourCC-style milestone tag for wave 9500. */
#define B9498_MILESTONE_TAG  0x4d393530u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9498_tag(void)
{
	return B9498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_9500 - report milestone fourCC tag for wave 9500.
 *
 * Always returns 0x4d393530 ("M950"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_9500(void)
{
	(void)NULL;
	return b9498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_9500(void)
    __attribute__((alias("gj_milestone_tag_9500")));
