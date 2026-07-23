/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12498: milestone 12500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_12500(void);
 *     - Returns the milestone tag for the milestone 12500 continuum
 *       (always 12500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_12500  (alias)
 *   __libcgj_batch12498_marker = "libcgj-batch12498"
 *
 * Milestone 12500 exclusive continuum CREATE-ONLY (12491-12500). Unique
 * gj_milestone_tag_12500 surface only; no multi-def. Distinct from
 * gj_graph_milestone_12500 (batch12500), gj_milestone_tag_12400
 * (batch12398), gj_milestone_tag_12300 (batch12298), and
 * gj_milestone_tag_12200 (batch12198). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12498_marker[] = "libcgj-batch12498";

/* Soft milestone tag for wave 12500. */
#define B12498_MILESTONE_TAG  12500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12498_tag(void)
{
	return B12498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_12500 - report milestone tag for wave 12500.
 *
 * Always returns 12500u. Soft pure-data only; does not parse ELF notes
 * or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_12500(void)
{
	(void)NULL;
	return b12498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_12500(void)
    __attribute__((alias("gj_milestone_tag_12500")));
