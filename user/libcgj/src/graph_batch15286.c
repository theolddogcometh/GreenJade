/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15286: continuum lane soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15286(void);
 *     - Returns the compile-time graph batch number for this TU (15286).
 *   uint32_t gj_lane_u_15286(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15286  (alias)
 *   uint32_t __gj_lane_u_15286  (alias)
 *   __libcgj_batch15286_marker = "libcgj-batch15286"
 *
 * Exclusive continuum CREATE-ONLY (15276-15300 wave toward MILESTONE
 * 15300). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15286_marker[] = "libcgj-batch15286";

/* Batch identity. */
#define B15286_ID  15286u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15286_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15286_id(void)
{
	return B15286_ID;
}

static uint32_t
b15286_lane(void)
{
	return B15286_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15286 - soft continuum surface
 *
 * Always returns 15286u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15286(void)
{
	(void)NULL;
	return b15286_id();
}

/*
 * gj_lane_u_15286 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_lane_u_15286(void)
{
	return b15286_lane();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15286(void)
    __attribute__((alias("gj_batch_id_15286")));

uint32_t __gj_lane_u_15286(void)
    __attribute__((alias("gj_lane_u_15286")));
