/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8398: milestone 8400 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_8400(void);
 *     - Returns the milestone 8400 continuum fourCC product tag
 *       (always 0x4D383430, fourCC "M840"). Soft compile-time
 *       product identity tag; not a runtime version probe.
 *   uint32_t __gj_milestone_tag_8400  (alias)
 *   __libcgj_batch8398_marker = "libcgj-batch8398"
 *
 * Milestone 8400 exclusive continuum CREATE-ONLY (8391-8400). Unique
 * gj_milestone_tag_8400 surface only; no multi-def. Distinct from
 * gj_product_score_8400 (batch8394), gj_continuum_wave_8400
 * (batch8399), and gj_graph_milestone_8400 (batch8400). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8398_marker[] = "libcgj-batch8398";

/* Milestone 8400 fourCC "M840" (big-endian ASCII). */
#define B8398_MILESTONE_TAG  0x4D383430u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8398_tag(void)
{
	return B8398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_8400 - report the milestone-8400 fourCC product tag.
 *
 * Always returns 0x4D383430 (fourCC "M840"). Soft pure-data only;
 * does not parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_8400(void)
{
	(void)NULL;
	return b8398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_8400(void)
    __attribute__((alias("gj_milestone_tag_8400")));
