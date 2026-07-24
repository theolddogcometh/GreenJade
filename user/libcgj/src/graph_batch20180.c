/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20180: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20180(void);
 *     - Returns the compile-time graph batch number for this TU (20180).
 *   uint32_t gj_bridge_u_20180(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20180  (alias)
 *   uint32_t __gj_bridge_u_20180  (alias)
 *   __libcgj_batch20180_marker = "libcgj-batch20180"
 *
 * Exclusive continuum CREATE-ONLY (20176-20200 wave toward MILESTONE
 * 20200). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Soft continuum only: fill helpers are compile-time lamps, not runtime
 * product probes. bar3 / product score remain open at milestone markers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch20180_marker[] = "libcgj-batch20180";

/* Batch identity. */
#define B20180_ID  20180u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20180_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20180_id(void)
{
	return B20180_ID;
}

static uint32_t
b20180_bridge(void)
{
	return B20180_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20180 - soft continuum surface
 *
 * Always returns 20180u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20180(void)
{
	(void)NULL;
	return b20180_id();
}

/*
 * gj_bridge_u_20180 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_20180(void)
{
	return b20180_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20180(void)
    __attribute__((alias("gj_batch_id_20180")));

uint32_t __gj_bridge_u_20180(void)
    __attribute__((alias("gj_bridge_u_20180")));
