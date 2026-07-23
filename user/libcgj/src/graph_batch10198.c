/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10198: milestone 10200 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_10200(void);
 *     - Returns the milestone fourCC tag for the milestone 10200
 *       continuum (always 0x4d313032, fourCC "M102"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_10200  (alias)
 *   __libcgj_batch10198_marker = "libcgj-batch10198"
 *
 * Milestone 10200 exclusive continuum CREATE-ONLY (10191-10200). Unique
 * gj_milestone_tag_10200 surface only; no multi-def. Distinct from
 * gj_graph_milestone_10200 (batch10200), gj_milestone_tag_10100
 * (batch10098), gj_milestone_tag_10000 (batch9998), and
 * gj_milestone_tag_9900 (batch9898). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10198_marker[] = "libcgj-batch10198";

/* Soft "M102" fourCC-style milestone tag for wave 10200. */
#define B10198_MILESTONE_TAG  0x4d313032u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10198_tag(void)
{
	return B10198_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_10200 - report milestone fourCC tag for wave 10200.
 *
 * Always returns 0x4d313032 ("M102"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_10200(void)
{
	(void)NULL;
	return b10198_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_10200(void)
    __attribute__((alias("gj_milestone_tag_10200")));
