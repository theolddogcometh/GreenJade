/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14708: milestone 14725 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14708(void);
 *     - Returns the compile-time graph batch number for this TU (14708).
 *   uint32_t gj_milestone_tag_14708(void);
 *     - Returns the milestone tag for the milestone 14725 continuum
 *       (always 14725u). Soft compile-time product identity tag.
 *   uint32_t __gj_batch_id_14708  (alias)
 *   uint32_t __gj_milestone_tag_14708  (alias)
 *   __libcgj_batch14708_marker = "libcgj-batch14708"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from gj_*_14700 / gj_*_14600 /
 * gj_*_14500 milestone surfaces and sibling 14725 milestone symbols.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14708_marker[] = "libcgj-batch14708";

/* Soft milestone tag for wave 14725. */
#define B14708_ID             14708u
#define B14708_MILESTONE_TAG  14725u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14708_id(void)
{
	return B14708_ID;
}

static uint32_t
b14708_tag(void)
{
	return B14708_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14708 - report this TU's graph batch number.
 *
 * Always returns 14708.
 */
uint32_t
gj_batch_id_14708(void)
{
	(void)NULL;
	return b14708_id();
}

/*
 * gj_milestone_tag_14708 - soft continuum surface
 *
 * Always returns 14725u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_milestone_tag_14708(void)
{
	return b14708_tag();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14708(void)
    __attribute__((alias("gj_batch_id_14708")));

uint32_t __gj_milestone_tag_14708(void)
    __attribute__((alias("gj_milestone_tag_14708")));
