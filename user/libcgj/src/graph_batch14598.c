/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14598: milestone 14600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_14600(void);
 *     - Returns the milestone tag for the milestone 14600 continuum
 *       (always 14600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_14600  (alias)
 *   __libcgj_batch14598_marker = "libcgj-batch14598"
 *
 * Milestone 14600 exclusive continuum CREATE-ONLY (14591-14600). Unique
 * gj_milestone_tag_14600 surface only; no multi-def. Distinct from gj_*_14500 /
 * gj_*_14400 / gj_*_14300 milestone surfaces and sibling 14600 milestone
 * symbols. No parent wires. Soft gates only (no Makefile / map / harness
 * wires in this TU).
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14598_marker[] = "libcgj-batch14598";

/* Soft milestone tag for wave 14600. */
#define B14598_MILESTONE_TAG  14600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14598_tag(void)
{
	return B14598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_14600 - milestone tag lamp for wave 14600
 *
 * Always returns 14600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_14600(void)
{
	(void)NULL;
	return b14598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_14600(void)
    __attribute__((alias("gj_milestone_tag_14600")));
