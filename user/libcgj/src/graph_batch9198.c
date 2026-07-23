/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9198: milestone 9200 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_9200(void);
 *     - Returns the milestone fourCC tag for the milestone 9200
 *       continuum (always 0x4d393230, fourCC "M920"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_9200  (alias)
 *   __libcgj_batch9198_marker = "libcgj-batch9198"
 *
 * Milestone 9200 exclusive continuum CREATE-ONLY (9191-9200). Unique
 * gj_milestone_tag_9200 surface only; no multi-def. Distinct from
 * gj_graph_milestone_9200 (batch9200), gj_milestone_tag_9100
 * (batch9098), gj_milestone_tag_9000 (batch8998), and
 * gj_milestone_tag_8900 (batch8898). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9198_marker[] = "libcgj-batch9198";

/* Soft "M920" fourCC-style milestone tag for wave 9200. */
#define B9198_MILESTONE_TAG  0x4d393230u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9198_tag(void)
{
	return B9198_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_9200 - report milestone fourCC tag for wave 9200.
 *
 * Always returns 0x4d393230 ("M920"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_9200(void)
{
	(void)NULL;
	return b9198_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_9200(void)
    __attribute__((alias("gj_milestone_tag_9200")));
