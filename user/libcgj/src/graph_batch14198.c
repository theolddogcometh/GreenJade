/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14198: milestone 14200 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_14200(void);
 *     - Returns the milestone tag for the milestone 14200 continuum
 *       (always 14200u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_14200  (alias)
 *   __libcgj_batch14198_marker = "libcgj-batch14198"
 *
 * Milestone 14200 exclusive continuum CREATE-ONLY (14191-14200). Unique
 * gj_milestone_tag_14200 surface only; no multi-def. Distinct from
 * gj_graph_milestone_14200 (batch14200), gj_milestone_tag_14000
 * (batch13998), gj_milestone_tag_13000 (batch12998), and
 * gj_milestone_tag_12900 (batch12898). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14198_marker[] = "libcgj-batch14198";

/* Soft milestone tag for wave 14200. */
#define B14198_MILESTONE_TAG  14200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14198_tag(void)
{
	return B14198_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_14200 - report milestone tag for wave 14200.
 *
 * Always returns 14200u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_14200(void)
{
	(void)NULL;
	return b14198_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_14200(void)
    __attribute__((alias("gj_milestone_tag_14200")));
