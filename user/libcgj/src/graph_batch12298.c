/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12298: milestone 12300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_12300(void);
 *     - Returns the milestone tag for the milestone 12300 continuum
 *       (always 12300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_12300  (alias)
 *   __libcgj_batch12298_marker = "libcgj-batch12298"
 *
 * Milestone 12300 exclusive continuum CREATE-ONLY (12291-12300). Unique
 * gj_milestone_tag_12300 surface only; no multi-def. Distinct from
 * gj_graph_milestone_12300 (batch12300), gj_milestone_tag_12200
 * (batch12198), gj_milestone_tag_12100 (batch12098), and
 * gj_milestone_tag_12000 (batch11998). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12298_marker[] = "libcgj-batch12298";

/* Soft milestone tag for wave 12300. */
#define B12298_MILESTONE_TAG  12300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12298_tag(void)
{
	return B12298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_12300 - report milestone tag for wave 12300.
 *
 * Always returns 12300u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_12300(void)
{
	(void)NULL;
	return b12298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_12300(void)
    __attribute__((alias("gj_milestone_tag_12300")));
