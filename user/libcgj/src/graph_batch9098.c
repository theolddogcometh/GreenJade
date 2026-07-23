/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9098: milestone 9100 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_9100(void);
 *     - Returns the milestone fourCC tag for the milestone 9100
 *       continuum (always 0x4d393130, fourCC "M910"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_9100  (alias)
 *   __libcgj_batch9098_marker = "libcgj-batch9098"
 *
 * Milestone 9100 exclusive continuum CREATE-ONLY (9091-9100). Unique
 * gj_milestone_tag_9100 surface only; no multi-def. Distinct from
 * gj_graph_milestone_9100 (batch9100), gj_milestone_tag_9000
 * (batch8998), gj_milestone_tag_8900 (batch8898), and
 * gj_milestone_tag_8800 (batch8798). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9098_marker[] = "libcgj-batch9098";

/* Soft "M910" fourCC-style milestone tag for wave 9100. */
#define B9098_MILESTONE_TAG  0x4d393130u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9098_tag(void)
{
	return B9098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_9100 - report milestone fourCC tag for wave 9100.
 *
 * Always returns 0x4d393130 ("M910"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_9100(void)
{
	(void)NULL;
	return b9098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_9100(void)
    __attribute__((alias("gj_milestone_tag_9100")));
