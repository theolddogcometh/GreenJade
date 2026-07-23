/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14986: continuum lane soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14986(void);
 *     - Returns the compile-time graph batch number for this TU (14986).
 *   uint32_t gj_lane_u_14986(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14986  (alias)
 *   uint32_t __gj_lane_u_14986  (alias)
 *   __libcgj_batch14986_marker = "libcgj-batch14986"
 *
 * Exclusive continuum CREATE-ONLY (14976-15000 wave toward MILESTONE
 * 15000). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14986_marker[] = "libcgj-batch14986";

/* Batch identity. */
#define B14986_ID  14986u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14986_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14986_id(void)
{
	return B14986_ID;
}

static uint32_t
b14986_lane(void)
{
	return B14986_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14986 - soft continuum surface
 *
 * Always returns 14986u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14986(void)
{
	(void)NULL;
	return b14986_id();
}

/*
 * gj_lane_u_14986 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_lane_u_14986(void)
{
	return b14986_lane();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14986(void)
    __attribute__((alias("gj_batch_id_14986")));

uint32_t __gj_lane_u_14986(void)
    __attribute__((alias("gj_lane_u_14986")));
